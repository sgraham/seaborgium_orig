// Copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "sg/backend/debug_core_gdb.h"

#include "base/logging.h"
#include "base/memory/scoped_ptr.h"
#include "base/string16.h"

DebugCoreGdb::DebugCoreGdb() {
}

DebugCoreGdb::~DebugCoreGdb() {
}

void DebugCoreGdb::ProcessStart(
    const string16& application,
    const string16& command_line,
    const std::vector<string16> environment,
    const string16& working_directory) {
  DCHECK_EQ(0, environment.size()) << "todo;";
  DCHECK_EQ(0, working_directory.size()) << "todo;";
}

DebugCoreGdb* DebugCoreGdb::CreateOnDBG() {
  return new DebugCoreGdb;
}
