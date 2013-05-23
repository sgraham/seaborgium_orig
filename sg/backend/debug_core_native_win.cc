// Copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "sg/backend/debug_core_native_win.h"

#include <windows.h>

#include "base/logging.h"
#include "base/memory/scoped_ptr.h"
#include "base/string16.h"
#include "sg/backend/backend_native_win.h"
#include "sg/backend/process_native_win.h"
#include "sg/debug_presenter_notify.h"

DebugCoreNativeWin::DebugCoreNativeWin() {
}

DebugCoreNativeWin::~DebugCoreNativeWin() {
}

void DebugCoreNativeWin::ProcessStart(
    const string16& application,
    const string16& command_line,
    const std::vector<string16> environment,
    const string16& working_directory) {
  DCHECK_EQ(0, environment.size()) << "todo;";

  // CreateProcessW can modify input buffer so we have to make a copy here.
  scoped_ptr<char16[]> command_line_copy(new char16[command_line.size() + 1]);
  command_line.copy(command_line_copy.get(), command_line.size());
  command_line_copy[command_line.size()] = 0;

  STARTUPINFO startup_information = {0};
  startup_information.cb = sizeof(STARTUPINFO);
  PROCESS_INFORMATION process_information = {0};
  /* TODO(handles)
  startup_info.dwFlags = STARTF_USESTDHANDLES;
  startup_info.hStdInput = nul;
  startup_info.hStdOutput = child_pipe;
  startup_info.hStdError = child_pipe;
  */
  BOOL result = CreateProcess(
      application.c_str(),
      command_line_copy.get(),
      NULL, NULL,
      FALSE,
      /*CREATE_SUSPENDED |*/ DEBUG_PROCESS | DETACHED_PROCESS,
      NULL,  // environment
      NULL,
      &startup_information,
      &process_information);
  CHECK(result);  // TODO(error): dispatch FormatLastError

  // TODO(temp):
  result = TerminateProcess(process_information.hProcess, 999);
  CHECK(result);
}

DebugCoreNativeWin* DebugCoreNativeWin::CreateOnDBG() {
  return new DebugCoreNativeWin;
}
