// Copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "sg/backend/backend_native_win.h"

DebugConnectionNativeWin::DebugConnectionNativeWin() {
}

DebugConnectionNativeWin::~DebugConnectionNativeWin() {
}

bool DebugConnectionNativeWin::GetName(string16* name, string16* err) {
  *name = L"Win32 (native)";
  return true;
}

Process* DebugConnectionNativeWin::ProcessCreate(
    const string16& application,
    const string16& command_line,
    const std::vector<string16> environment,
    const string16& working_directory,
    string16* err) {
  return NULL;
}

void DebugConnectionNativeWin::GetProcessList(std::vector<ProcessId>* result) {
  result->clear();
}

Process* DebugConnectionNativeWin::ProcessAttach(
    ProcessId process_id, string16* err) {
  return NULL;
}
