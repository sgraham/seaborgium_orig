// Copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <gtest/gtest.h>

#include <memory>

#include "sg/backend/gdb_mi_parse.h"
#include "sg/test.h"

class GdbMiParse : public LeakCheckTest {
};

TEST(GdbMiParse, Welcome) {
  GdbMiParser p;

  std::unique_ptr<GdbRecord> welcome(p.Parse("~\"GNU gdb (GDB) 7.5\\n\"\r", NULL));
  EXPECT_EQ(GdbRecord::RT_CONSOLE_STREAM_OUTPUT, welcome->record_type());
  EXPECT_EQ("GNU gdb (GDB) 7.5\n", welcome->OutputString());
}

TEST(GdbMiParse, LogData) {
  GdbMiParser p;
  std::unique_ptr<GdbRecord> diag(p.Parse("&\"set disassembly-flavor intel\\n\"\r",
                                     NULL));
  EXPECT_EQ(GdbRecord::RT_LOG_STREAM_OUTPUT, diag->record_type());
  EXPECT_EQ("set disassembly-flavor intel\n", diag->OutputString());
}

TEST(GdbMiParse, ResultDone) {
  GdbMiParser p;
  std::unique_ptr<GdbRecord> done(p.Parse("^done\r", NULL));
  EXPECT_EQ(GdbRecord::RT_RESULT_RECORD, done->record_type());
  EXPECT_EQ("done", done->ResultClass());
  EXPECT_EQ(0, done->results().size());
}

TEST(GdbMiParse, ResultDoneSimple) {
  GdbMiParser p;
  std::unique_ptr<GdbRecord> done(p.Parse("^done,value=\"42.432000000000002\"\r",
                                     NULL));
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
  std::unique_ptr<GdbRecord> done(p.Parse(
      "^error,msg=\"Undefined info command: \\\"regs\\\"."
      "  Try \\\"help info\\\".\"\r",
      NULL));
  EXPECT_EQ(GdbRecord::RT_RESULT_RECORD, done->record_type());
  EXPECT_EQ("error", done->ResultClass());
  EXPECT_EQ(1, done->results().size());
  EXPECT_EQ("msg", done->results()[0]->variable());
  std::string value;
  EXPECT_TRUE(done->results()[0]->value()->GetAsString(&value));
  EXPECT_EQ("Undefined info command: \"regs\".  Try \"help info\".", value);
}

TEST(GdbMiParse, ResultDoneTuple) {
  GdbMiParser p;
  std::unique_ptr<GdbRecord> done(p.Parse(
      "^done,stuff={a=\"stuff\",b=\"things\"}\r", NULL));
  EXPECT_EQ(GdbRecord::RT_RESULT_RECORD, done->record_type());
  EXPECT_EQ("done", done->ResultClass());
  EXPECT_EQ(1, done->results().size());
  EXPECT_EQ("stuff", done->results()[0]->variable());
  base::Value* stuff = done->results()[0]->value();
  base::DictionaryValue* as_dict;
  EXPECT_TRUE(stuff->GetAsDictionary(&as_dict));
  std::string a_value, b_value;
  EXPECT_TRUE(as_dict->GetString("a", &a_value));
  EXPECT_TRUE(as_dict->GetString("b", &b_value));
  EXPECT_EQ("stuff", a_value);
  EXPECT_EQ("things", b_value);
}

TEST(GdbMiParse, ResultDoneListOfTuple) {
  GdbMiParser p;
  std::unique_ptr<GdbRecord> done(p.Parse(
      "^done,asm_insns=["
      "{address=\"0x004013cb\","
       "func-name=\"main(int, char**)\","
       "offset=\"63\","
       "inst=\"mov    DWORD PTR [esp+0x1c],0x0\""
      "},"
       "{address=\"0x004013d3\","
        "func-name=\"main(int, char**)\","
        "offset=\"71\","
        "inst=\"jmp    0x4013fd <main(int, char**)+113>\""
      "}]\r", NULL));
  EXPECT_EQ(GdbRecord::RT_RESULT_RECORD, done->record_type());
}

TEST(GdbMiParse, FullOutput) {
  GdbMiReader reader;
  std::unique_ptr<GdbOutput> output(reader.Parse(
      "=thread-group-added,id=\"i1\"\r"
      "~\"GNU gdb (GDB) 7.5\\n\"\r"
      "~\"<http blahblah\"\r"
      "(gdb) \r", NULL));
  EXPECT_EQ(3, output->size());
  EXPECT_EQ(GdbRecord::RT_NOTIFY_ASYNC_OUTPUT, output->at(0)->record_type());
  EXPECT_EQ("thread-group-added", output->at(0)->AsyncClass());
  EXPECT_EQ(1, output->at(0)->results().size());
  EXPECT_EQ("id", output->at(0)->results()[0]->variable());
  EXPECT_EQ(GdbRecord::RT_CONSOLE_STREAM_OUTPUT, output->at(1)->record_type());
  EXPECT_EQ("GNU gdb (GDB) 7.5\n", output->at(1)->OutputString());
  EXPECT_EQ(GdbRecord::RT_CONSOLE_STREAM_OUTPUT, output->at(2)->record_type());
  EXPECT_EQ("<http blahblah", output->at(2)->OutputString());
}

TEST(GdbMiParse, Incremental) {
  GdbMiReader reader;
  int num_bytes;

  std::string current = "=thread-group-added,id=\"";
  std::unique_ptr<GdbOutput> output(reader.Parse(current, &num_bytes));
  EXPECT_EQ(NULL, output.get());
  EXPECT_EQ(0, num_bytes);

  current += "i1\"";
  output.reset(reader.Parse(current, &num_bytes));
  EXPECT_EQ(NULL, output.get());
  EXPECT_EQ(0, num_bytes);

  current += "\r";
  output.reset(reader.Parse(current, &num_bytes));
  EXPECT_EQ(NULL, output.get());
  EXPECT_EQ(0, num_bytes);

  current += "(gdb) \r";
  output.reset(reader.Parse(current, &num_bytes));
  EXPECT_EQ(35, num_bytes);
  EXPECT_EQ(1, output->size());
  EXPECT_EQ(GdbRecord::RT_NOTIFY_ASYNC_OUTPUT, output->at(0)->record_type());
  EXPECT_EQ("thread-group-added", output->at(0)->AsyncClass());
  EXPECT_EQ(1, output->at(0)->results().size());
  EXPECT_EQ("id", output->at(0)->results()[0]->variable());
}

TEST(GdbMiParse, Multiple) {
  GdbMiReader reader;
  int num_bytes;

  std::string full = "=thread-group-added,id=\"i1\"\r(gdb) \r"
                     "~\"<http blahblah\"\r(gdb) \r";
  base::StringPiece current(full);
  std::unique_ptr<GdbOutput> output(reader.Parse(current, &num_bytes));
  EXPECT_EQ(35, num_bytes);
  EXPECT_EQ(1, output->size());
  EXPECT_EQ(GdbRecord::RT_NOTIFY_ASYNC_OUTPUT, output->at(0)->record_type());
  EXPECT_EQ("thread-group-added", output->at(0)->AsyncClass());

  current = base::StringPiece(
      current.data() + num_bytes, current.size() - num_bytes);
  output.reset(reader.Parse(current, &num_bytes));
  EXPECT_EQ(25, num_bytes);
  EXPECT_EQ(1, output->size());
  EXPECT_EQ(GdbRecord::RT_CONSOLE_STREAM_OUTPUT, output->at(0)->record_type());
  EXPECT_EQ("<http blahblah", output->at(0)->OutputString());
}

TEST(GdbMiParse, LineEndings) {
  GdbMiReader reader;
  int num_bytes;

  std::string full = "=thread-group-added,id=\"i1\"\r(gdb) \r\n";
  base::StringPiece current(full);
  std::unique_ptr<GdbOutput> output(reader.Parse(current, &num_bytes));
  EXPECT_EQ(36, num_bytes);
  EXPECT_EQ(1, output->size());
  EXPECT_EQ(GdbRecord::RT_NOTIFY_ASYNC_OUTPUT, output->at(0)->record_type());
  EXPECT_EQ("thread-group-added", output->at(0)->AsyncClass());
}

TEST(GdbMiParse, IncrementalWithMultibyteLineEnding) {
  GdbMiReader reader;
  int num_bytes;

  std::string initial = "=thread-group-added,id=\"i1\"\r(gdb) \r";
  base::StringPiece current(initial);
  std::unique_ptr<GdbOutput> output(reader.Parse(current, &num_bytes));
  EXPECT_EQ(35, num_bytes);
  EXPECT_EQ(1, output->size());
  EXPECT_EQ(GdbRecord::RT_NOTIFY_ASYNC_OUTPUT, output->at(0)->record_type());
  EXPECT_EQ("thread-group-added", output->at(0)->AsyncClass());

  // Note \n from previous output is ambiguous because newline can be either
  // style. So, make sure we skip it in that position.
  std::string rest = "\n~\"<http blahblah\"\r(gdb) \r\n";
  output.reset(reader.Parse(rest, &num_bytes));
  EXPECT_EQ(27, num_bytes);
  EXPECT_EQ(1, output->size());
  EXPECT_EQ(GdbRecord::RT_CONSOLE_STREAM_OUTPUT, output->at(0)->record_type());
  EXPECT_EQ("<http blahblah", output->at(0)->OutputString());
}

TEST(GdbMiParse, FailingVariableUpdate) {
  GdbMiReader reader;
  int num_bytes;
  std::string str = "20^done,changelist=[{name=\"V1\",value=\"0x522c68 "
                    "\\\"\\\\r\\360\\255\\272\\\\r\\360\\255\\272\\\"\","
                    "in_scope=\"true\",type_changed=\"false\",has_more=\"0\"}]"
                    "\r(gdb) \r";
  std::unique_ptr<GdbOutput> output(reader.Parse(str, &num_bytes));
  EXPECT_NE(static_cast<GdbOutput*>(NULL), output.get());
  EXPECT_EQ(1, output->size());
  EXPECT_EQ(GdbRecord::RT_RESULT_RECORD, output->at(0)->record_type());
  EXPECT_EQ("20", output->at(0)->token());
  EXPECT_EQ("done", output->at(0)->ResultClass());
  EXPECT_EQ("changelist", output->at(0)->results()[0]->variable());
  base::ListValue* list_value;
  EXPECT_TRUE(output->at(0)->results()[0]->value()->GetAsList(&list_value));
  base::DictionaryValue* dict_value;
  EXPECT_TRUE(list_value->GetDictionary(0, &dict_value));
  string16 name_str, value_str;
  EXPECT_TRUE(dict_value->GetStringWithoutPathExpansion("name", &name_str));
  EXPECT_EQ(L"V1", name_str);
  EXPECT_TRUE(dict_value->GetStringWithoutPathExpansion("value", &value_str));
  // TODO(scottmg): I'm not actually sure if this decode is right.
  EXPECT_EQ(L"0x522c68 \"\\r\xEA0\xE5D\xE6A\\r\xEA0\xE5D\xE6A\"", value_str);
}

// TODO(testing): Bad/unexpected outputs.
