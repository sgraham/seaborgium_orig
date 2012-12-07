// Copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef SG_BACKEND_GDB_MI_PARSE_H_
#define SG_BACKEND_GDB_MI_PARSE_H_

#include <string>

#include "base/memory/scoped_ptr.h"
#include "base/string_piece.h"
#include "base/values.h"

namespace base {
class ListValue;
}

// Helpers to parse GDB MI2 output records. It's not clear from the docs what
// the string format is. Some places explicitly say 7-bit strings, so until
// demonstrated otherwise, the strings and identifiers are parsed as
// no-high-bit ASCII only.

class GdbRecord {
 public:
  ~GdbRecord();

  enum RecordType {
    RT_EXEC_ASYNC_OUTPUT,
    RT_STATUS_ASYNC_OUTPUT,
    RT_NOTIFY_ASYNC_OUTPUT,
    RT_CONSOLE_STREAM_OUTPUT,
    RT_TARGET_STREAM_OUTPUT,
    RT_LOG_STREAM_OUTPUT,
    RT_RESULT_RECORD,
  };

  RecordType record_type() const { return record_type_; }
  const std::string& token() const { return token_; }

  // This is the 'result-class' for results, the string data for
  // '*-stream-output', and 'async-class' for the 'async-output' commands.
  const std::string& primary_identifier() const { return primary_identifier_; }

  // Some alternate names for |primary_identifier| that are more natural
  // for the types.
  const std::string& OutputString() const { return primary_identifier(); }
  const std::string& AsyncClass() const { return primary_identifier(); }
  const std::string& ResultClass() const { return primary_identifier(); }

  // This is used for result-record and the async-output types and represents
  // their list of results.
  const base::ListValue& results() const { return results_; }

 private:
  friend class GdbMiParser;

  explicit GdbRecord(RecordType record_type);
  GdbRecord(RecordType record_type, const std::string& token);

  void set_primary_identifier(const std::string& identifier) {
    primary_identifier_ = identifier;
  }

  RecordType record_type_;
  std::string token_;  // Optional string of digits corresponding with input.
  std::string primary_identifier_;

  base::ListValue results_;

  DISALLOW_COPY_AND_ASSIGN(GdbRecord);
};

class GdbMiParser {
 public:
  GdbMiParser();
  ~GdbMiParser();

  // Returns a parsed GdbRecord (one line of communication). Caller owns.
  GdbRecord* Parse(const base::StringPiece& input);

 private:
  enum Token {
    T_ID_TOKEN,
    T_VARIABLE,         // Unquoted.
    T_STRING,           // C-style quoted.
    T_TUPLE_BEGIN,      // {
    T_TUPLE_END,        // }
    T_LIST_BEGIN,       // [
    T_LIST_END,         // ]
    T_SEPARATOR,        // ,
    T_TERMINATOR,
  };

  GdbRecord* DetermineTypeAndMakeRecord();
  std::string GetTokenIfAny();

  void ReportError();

  bool CanConsume(int count);

  std::string ConsumeString();
  std::string ConsumeIdentifier();
  void ConsumeNewline();

  // Pointer to the start of the input data.
  const char* start_pos_;

  // Pointer to the current position in the input data.
  const char* pos_;

  // Pointer to one-past the last character of the input data.
  const char* end_pos_;

  // |true| if an error has been encountered.
  bool error_;

  // Offset at which the error was encountered, if |error_| is set.
  int error_index_;

  DISALLOW_COPY_AND_ASSIGN(GdbMiParser);
};

#endif  // SG_BACKEND_GDB_MI_PARSE_H_
