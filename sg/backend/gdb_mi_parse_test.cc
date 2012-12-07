// Copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <gtest/gtest.h>

#include "gdb_mi_parse.h"

TEST(GdbMiParse, Welcome) {
  GdbMiParser p;

  scoped_ptr<GdbRecord> welcome(p.Parse("~\"GNU gdb (GDB) 7.5\\n\""));
  EXPECT_EQ(GdbRecord::RT_CONSOLE_STREAM_OUTPUT, welcome->record_type());
  EXPECT_EQ("GNU gdb (GDB) 7.5\n", welcome->OutputString());
}

TEST(GdbMiParse, LogData) {
  GdbMiParser p;
  scoped_ptr<GdbRecord> diag(p.Parse("&\"set disassembly-flavor intel\\n\""));
  EXPECT_EQ(GdbRecord::RT_LOG_STREAM_OUTPUT, diag->record_type());
  EXPECT_EQ("set disassembly-flavor intel\n", diag->OutputString());
}
