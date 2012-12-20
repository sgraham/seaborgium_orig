// Copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <gtest/gtest.h>

#include <string>

#include "base/at_exit.h"
#include "base/memory/weak_ptr.h"
#include "base/message_loop.h"
#include "base/run_loop.h"
#include "base/synchronization/waitable_event.h"
#include "base/task_runner_util.h"
#include "base/threading/thread.h"
#include "base/threading/thread_restrictions.h"
#include "sg/app_thread.h"
#include "sg/backend/debug_core_gdb.h"
#include "sg/main_loop.h"

class MockNotifier : public DebugNotification {
 public:
  virtual ~MockNotifier() {}

  void ExpectLoadProcess() {
    next_event_ = "loaded";
  }

  // Implementation of DebugNotification:
  virtual void OnProcessLoaded() {
    Check("loaded");
  }

 private:
  void Check(const std::string& event) {
    EXPECT_EQ(event, next_event_);
    next_event_ = "";
  }

  std::string next_event_;
};

class DebugCoreGdbWithAppThreads : public testing::Test {
 public:
  void SetUp() {
    main_loop.Init();
    main_loop.MainMessageLoopStart();
    main_loop.CreateThreads();
  }
  void TearDown() {
    main_loop.ShutdownThreadsAndCleanUp();
  }
  void Run() {
    main_loop.MainMessageLoopRun();
  }

  MainLoop main_loop;
};

void StartAndStopImmediately(base::WeakPtr<DebugCoreGdb> debug_core) {
  AppThread::PostTask(AppThread::BACKEND, FROM_HERE,
      base::Bind(&DebugCoreGdb::LoadProcess,
                 debug_core,
                 // TODO(scottmg): Relative to gdb location.
                 L"../test_data/test_binary_mingw.exe",
                 L"", std::vector<string16>(), L""));

  AppThread::PostTask(AppThread::BACKEND, FROM_HERE,
      base::Bind(&DebugCoreGdb::StopDebugging, debug_core));

  AppThread::PostTask(AppThread::BACKEND, FROM_HERE,
      base::Bind(&DebugCoreGdb::DeleteSelf, debug_core));

  MessageLoopForUI::current()->Quit();
}

TEST_F(DebugCoreGdbWithAppThreads, StartAndStopImmediately) {
  AppThread::PostTaskAndReplyWithResult(
      AppThread::BACKEND, FROM_HERE,
      base::Bind(&DebugCoreGdb::Create),
      base::Bind(&StartAndStopImmediately));
  Run();
}

void RunUntilMainShutdown(base::WeakPtr<DebugCoreGdb> debug_core) {
  AppThread::PostTask(AppThread::BACKEND, FROM_HERE,
      base::Bind(&DebugCoreGdb::StopDebugging, debug_core));
  AppThread::PostTask(AppThread::BACKEND, FROM_HERE,
      base::Bind(&DebugCoreGdb::DeleteSelf, debug_core));
  MessageLoopForUI::current()->Quit();
}

class RunUntilMainNotifier : public DebugNotification {
 public:
  RunUntilMainNotifier() {}
  virtual ~RunUntilMainNotifier() {}
  virtual void OnStoppedAtBreakpoint(const StoppedAtBreakpointData& data) {
    EXPECT_EQ(L"main", data.frame.function);
    EXPECT_EQ(L"test_binary.cc", data.frame.filename);
    AppThread::PostTask(AppThread::UI, FROM_HERE,
        base::Bind(&RunUntilMainShutdown, debug_core));
  }
  base::WeakPtr<DebugCoreGdb> debug_core;
};

void StartAndRunUntilMain(
    RunUntilMainNotifier* notifier,
    base::WeakPtr<DebugCoreGdb> debug_core) {
  notifier->debug_core = debug_core;
  AppThread::PostTask(AppThread::BACKEND, FROM_HERE,
      base::Bind(&DebugCoreGdb::SetDebugNotification,
                 debug_core,
                 notifier));

  AppThread::PostTask(AppThread::BACKEND, FROM_HERE,
      base::Bind(&DebugCoreGdb::LoadProcess,
                 debug_core,
                 // TODO(scottmg): Relative to gdb location.
                 L"../test_data/test_binary_mingw.exe",
                 L"", std::vector<string16>(), L""));

  AppThread::PostTask(AppThread::BACKEND, FROM_HERE,
      base::Bind(&DebugCoreGdb::RunToMain,
                 debug_core));
}

TEST_F(DebugCoreGdbWithAppThreads, StartAndRunUntilMain) {
  RunUntilMainNotifier notifier;
  AppThread::PostTaskAndReplyWithResult(
      AppThread::BACKEND, FROM_HERE,
      base::Bind(&DebugCoreGdb::Create),
      base::Bind(&StartAndRunUntilMain, &notifier));
  Run();
}
