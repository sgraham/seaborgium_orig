// Copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef SG_BACKEND_DEBUG_CONNECTION_NATIVE_WIN_H_
#define SG_BACKEND_DEBUG_CONNECTION_NATIVE_WIN_H_

#include <vector>

#include "sg/backend/backend.h"

class DebugPresenterNotify;

// An implementation of a debugger backend using Win32 APIs. Note: Requires a
// smattering of helper DLLs to be available.

class DebugCoreNativeWin /* : public DebugCore */ {
 public:
  DebugCoreNativeWin();
  virtual ~DebugCoreNativeWin();

  // Implementation of DebugCore:
  virtual void ProcessStart(
      const string16& application,
      const string16& command_line,
      const std::vector<string16> environment,
      const string16& working_directory);

  static DebugCoreNativeWin* CreateOnDBG();

 private:

  DISALLOW_COPY_AND_ASSIGN(DebugCoreNativeWin);
};

#endif  // SG_BACKEND_DEBUG_CONNECTION_NATIVE_WIN_H_
