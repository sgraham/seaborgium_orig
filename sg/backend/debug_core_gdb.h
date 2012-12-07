// Copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef SG_BACKEND_DEBUG_CORE_GDB_H_
#define SG_BACKEND_DEBUG_CORE_GDB_H_

#include <vector>

#include "sg/backend/backend.h"

// An implementation of a debugger backend using GDB/MI.

class DebugCoreGdb /* : public DebugCore */ {
 public:
  DebugCoreGdb();
  virtual ~DebugCoreGdb();

  // Implementation of DebugCore:
  virtual void SetDebugNotification(DebugNotification* debug_notification);

  virtual void LoadProcess(
      const string16& application,
      const string16& command_line,
      const std::vector<string16> environment,
      const string16& working_directory);

  static DebugCoreGdb* CreateOnDBG();

 private:
  DebugNotification* debug_notification_;

  DISALLOW_COPY_AND_ASSIGN(DebugCoreGdb);
};

#endif  // SG_BACKEND_DEBUG_CORE_GDB_H_
