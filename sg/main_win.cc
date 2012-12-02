// Copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/at_exit.h"
#include "base/command_line.h"
#include "sg/application_window.h"
#include "sg/main_loop.h"

int RunMain(HINSTANCE instance) {
  scoped_ptr<MainLoop> main_loop(new MainLoop);
  main_loop->Init();
  main_loop->MainMessageLoopStart();
  main_loop->CreateThreads();
  scoped_ptr<ApplicationWindow> app_window(ApplicationWindow::Create());
  main_loop->MainMessageLoopRun();
  app_window.reset();
  main_loop->ShutdownThreadsAndCleanUp();
  return main_loop->GetResultCode();
}

int APIENTRY wWinMain(HINSTANCE instance, HINSTANCE prev, wchar_t*, int) {
  _CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_FILE | _CRTDBG_MODE_DEBUG);
  _CrtSetReportFile(_CRT_WARN, _CRTDBG_FILE_STDERR);
  _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
  // _CrtSetBreakAlloc(85175);

  CommandLine::Init(0, NULL);
  base::AtExitManager exit_manager;
  return RunMain(instance);
}
