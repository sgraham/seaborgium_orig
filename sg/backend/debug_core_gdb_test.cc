// Copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <gtest/gtest.h>

#include "base/at_exit.h"
#include "base/memory/weak_ptr.h"
#include "base/message_loop.h"
#include "base/run_loop.h"
#include "base/synchronization/waitable_event.h"
#include "base/task_runner_util.h"
#include "base/threading/thread.h"
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
                 L"test_data/test_binary_mingw.exe",
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
