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

TEST(GdbMiParse, ResultDone) {
  GdbMiParser p;
  scoped_ptr<GdbRecord> done(p.Parse("^done\r"));
  EXPECT_EQ(GdbRecord::RT_RESULT_RECORD, done->record_type());
  EXPECT_EQ("done", done->ResultClass());
  EXPECT_EQ(0, done->results().size());
}

TEST(GdbMiParse, ResultDoneSimple) {
  GdbMiParser p;
  scoped_ptr<GdbRecord> done(p.Parse("^done,value=\"42.432000000000002\"\r"));
  EXPECT_EQ(GdbRecord::RT_RESULT_RECORD, done->record_type());
  EXPECT_EQ("done", done->ResultClass());
  EXPECT_EQ(1, done->results().size());
  EXPECT_EQ("value", done->results()[0]->variable());
  std::string value;
  EXPECT_TRUE(done->results()[0]->value()->GetAsString(&value));
  EXPECT_EQ("42.432000000000002", value);
}

TEST(GdbMiParse, ResultErrorSimple) {
  GdbMiParser p;
  scoped_ptr<GdbRecord> done(p.Parse(
      "^error,msg=\"Undefined info command: \\\"regs\\\"."
      "  Try \\\"help info\\\".\"\r"));
  EXPECT_EQ(GdbRecord::RT_RESULT_RECORD, done->record_type());
  EXPECT_EQ("error", done->ResultClass());
  EXPECT_EQ(1, done->results().size());
  EXPECT_EQ("msg", done->results()[0]->variable());
  std::string value;
  EXPECT_TRUE(done->results()[0]->value()->GetAsString(&value));
  EXPECT_EQ("Undefined info command: \"regs\".  Try \"help info\".", value);
}

// TODO(testing): results, errors
