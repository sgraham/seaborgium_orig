// Copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <gtest/gtest.h>

#include <windows.h>

class DebugCoreNativeWinTest : public testing::Test {
 public:
  DebugCoreNativeWinTest() {
  }

  //scoped_ptr<Debugger> debugger_;
};

TEST_F(DebugCoreNativeWinTest, CreateOK) {
  STARTUPINFO startup_info = {0};
  startup_info.cb = sizeof(STARTUPINFO);
  PROCESS_INFORMATION process_information = {0};
  /* TODO(handles)
  startup_info.dwFlags = STARTF_USESTDHANDLES;
  startup_info.hStdInput = nul;
  startup_info.hStdOutput = child_pipe;
  startup_info.hStdError = child_pipe;
  */
  BOOL result;
  result = CreateProcess(
      L"test_data/test_binary.exe",
      NULL,
      NULL, NULL,
      FALSE,
      /*CREATE_SUSPENDED |*/ DEBUG_PROCESS | DETACHED_PROCESS,
      NULL,  // environment
      NULL,
      &startup_info,
      &process_information);
  EXPECT_TRUE(result);

  // Immediately terminate it. We'll still get a create and a load dll for
  // ntdll.dll.
  result = TerminateProcess(process_information.hProcess, 999);
  EXPECT_TRUE(result);

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
