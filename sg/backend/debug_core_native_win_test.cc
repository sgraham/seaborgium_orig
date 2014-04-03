// Copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <gtest/gtest.h>

#include <memory>

#include "base/at_exit.h"
#include "base/run_loop.h"
#include "base/synchronization/waitable_event.h"
#include "base/task_runner_util.h"
#include "base/threading/thread.h"
#include "sg/backend/debug_core_native_win.h"

class DebugCoreNativeWinTest : public testing::Test {
 public:
  DebugCoreNativeWinTest() {
    MessageLoopForUI main_message_loop;
    debug_thread_.reset(new base::Thread("DBG"));
    debug_thread_->Start();
    base::PostTaskAndReplyWithResult(
        debug_thread_->message_loop_proxy(),
        FROM_HERE,
        base::Bind(&DebugCoreNativeWin::CreateOnDBG),
        base::Bind(&DebugCoreNativeWinTest::OnCreate, base::Unretained(this)));
    main_message_loop.Run();
  }

  void OnCreate(DebugCoreNativeWin* new_core) {
    debug_core_ = new_core;
    MessageLoopForUI::current()->Quit();
  }

  virtual ~DebugCoreNativeWinTest() {
  }

  DebugCoreNativeWin* debug_core_;
  std::unique_ptr<base::Thread> debug_thread_;
};

TEST_F(DebugCoreNativeWinTest, DISABLED_StartAndKillImmediately) {
  debug_core_->ProcessStart(
      L"test_data\\test_binary.exe", L"", std::vector<string16>(), L"");

  // XXX XXX XXX

  /*
  // Immediately terminate it. We'll still get a create and a load dll for
  // ntdll.dll.

  debug_core_->GetDebugEvents();
  DEBUG_EVENT debug_event = {0};
  result = WaitForDebugEvent(&debug_event, INFINITE);
  EXPECT_TRUE(result);
  EXPECT_EQ(CREATE_PROCESS_DEBUG_EVENT, debug_event.dwDebugEventCode);
  ContinueDebugEvent(debug_event.dwProcessId,
                    debug_event.dwThreadId,
                    DBG_CONTINUE);

  result = WaitForDebugEvent(&debug_event, INFINITE);
  EXPECT_TRUE(result);
  EXPECT_EQ(LOAD_DLL_DEBUG_EVENT, debug_event.dwDebugEventCode);
  ContinueDebugEvent(debug_event.dwProcessId,
                    debug_event.dwThreadId, 
                    DBG_CONTINUE);

  result = WaitForDebugEvent(&debug_event, INFINITE);
  EXPECT_TRUE(result);
  EXPECT_EQ(EXIT_PROCESS_DEBUG_EVENT, debug_event.dwDebugEventCode);
  EXPECT_EQ(999, debug_event.u.ExitProcess.dwExitCode);
*/

  /*
  debugger_->ProcessStart(
      L"test_data/test_binary.exe", L"", std::vector<string16>(), L"");
  */
  // PostTask to DebugCoreNativeWin
  // DebugCoreNativeWin::ProcessStart will
  // - CreateProcess
  // - PumpDebugEvents
  //
  // PumpDebugEvents:
  // - WaitForDebugEvent(0)
  // - if (any)
  //     PostTask(listener)
  //   else
  //     PostDelayedTask(PumpDebugEvents, WeakPtr<this>, xx);

  // Test has to wait until it receives the correct event or times out, which
  // sucks.

  // Do the tests directly on DebugCoreNativeWin
  //
  // Still have to poll. Could have it WFDE(INFINITE), but then code execution
  // would be quite different.
  //
  //
}
