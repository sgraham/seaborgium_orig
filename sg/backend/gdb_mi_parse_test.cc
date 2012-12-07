// Copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <gtest/gtest.h>

#include "sg/backend/gdb_mi_parse.h"

TEST(GdbMiParse, Welcome) {
  GdbMiParser p;

  scoped_ptr<GdbRecord> welcome(p.Parse("~\"GNU gdb (GDB) 7.5\\n\"\r"));
  EXPECT_EQ(GdbRecord::RT_CONSOLE_STREAM_OUTPUT, welcome->record_type());
  EXPECT_EQ("GNU gdb (GDB) 7.5\n", welcome->OutputString());
}

TEST(GdbMiParse, LogData) {
  GdbMiParser p;
  scoped_ptr<GdbRecord> diag(p.Parse("&\"set disassembly-flavor intel\\n\"\r"));
  EXPECT_EQ(GdbRecord::RT_LOG_STREAM_OUTPUT, diag->record_type());
  EXPECT_EQ("set disassembly-flavor intel\n", diag->OutputString());
}

TEST(GdbMiParse, Done) {
  GdbMiParser p;
  scoped_ptr<GdbRecord> done(p.Parse("^done\r"));
  EXPECT_EQ(GdbRecord::RT_RESULT_RECORD, done->record_type());
  EXPECT_EQ(0, done->results().GetSize());
}

// TODO: results, errors
