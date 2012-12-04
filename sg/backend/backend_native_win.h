// Copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef SG_BACKEND_BACKEND_NATIVE_WIN_H_
#define SG_BACKEND_BACKEND_NATIVE_WIN_H_

#include <vector>

#include "sg/backend/backend.h"

class DebugPresenterNotify;

// An implementation of a debugger backend using Win32 APIs. Note: Requires a
// smattering of helper DLLs to be available.

class DebugConnectionNativeWin : public DebugConnection {
 public:
  explicit DebugConnectionNativeWin(
      DebugPresenterNotify* debug_presenter_notify);
  virtual ~DebugConnectionNativeWin();

  // Implementation of DebugConnection:
  virtual bool GetName(string16* name, string16* error);
  virtual Process* ProcessCreate(
      const string16& application,
      const string16& command_line,
      const std::vector<string16> environment,
      const string16& working_directory,
      string16* error);
  virtual void GetProcessList(std::vector<ProcessId>* result);
  virtual Process* ProcessAttach(ProcessId process_id, string16* error);

 private:
  DebugPresenterNotify* debug_presenter_notify_;

  DISALLOW_COPY_AND_ASSIGN(DebugConnectionNativeWin);
};

#endif  // SG_BACKEND_BACKEND_NATIVE_WIN_H_
