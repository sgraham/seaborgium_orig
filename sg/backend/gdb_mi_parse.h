// Copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef SG_BACKEND_GDB_MI_PARSE_H_
#define SG_BACKEND_GDB_MI_PARSE_H_

#include <memory>
#include <string>
#include <vector>

#include "base/logging.h"
#include "base/string_piece.h"
#include "base/values.h"

namespace base {
class ListValue;
}

// Helpers to parse GDB MI2 output records. It's not clear from the docs what
// the string format is. Some places explicitly say 7-bit strings, so until
// demonstrated otherwise, the strings and identifiers are parsed as
// no-high-bit ASCII only.
//
// Documentation from:
// http://sourceware.org/gdb/onlinedocs/gdb/GDB_002fMI-Output-Syntax.html#GDB_002fMI-Output-Syntax
// titled "GDB/MI Output Syntax" (in case it moves). It is slightly inaccurate
// as noted in few locations in the implementation.

class GdbRecordResult {
 public:
  GdbRecordResult();
  ~GdbRecordResult();
  const std::string& variable() const { return variable_; }
  base::Value* value() const { return value_.get(); }

 private:
  friend class GdbMiParser;

  void set_variable(const std::string& variable) { variable_ = variable; }
  void set_value(base::Value* value) { value_.reset(value); }
  base::Value* release_value() { return value_.release(); }

  std::string variable_;
  std::unique_ptr<base::Value> value_;

  DISALLOW_COPY_AND_ASSIGN(GdbRecordResult);
};

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
    RT_TERMINATOR,
  };

  RecordType record_type() const { return record_type_; }
  const std::string& token() const { return token_; }

  // This is the 'result-class' for results, the string data for
  // '*-stream-output', and 'async-class' for the 'async-output' commands.
  const std::string& primary_identifier() const { return primary_identifier_; }

  // Some alternate names for |primary_identifier| that are more natural
  // for the types.
  const std::string& OutputString() const {
    DCHECK(record_type() == RT_CONSOLE_STREAM_OUTPUT ||
           record_type() == RT_TARGET_STREAM_OUTPUT ||
           record_type() == RT_LOG_STREAM_OUTPUT);
    return primary_identifier();
  }
  const std::string& AsyncClass() const {
    DCHECK(record_type() == RT_EXEC_ASYNC_OUTPUT ||
           record_type() == RT_STATUS_ASYNC_OUTPUT ||
           record_type() == RT_NOTIFY_ASYNC_OUTPUT);
    return primary_identifier();
  }
  const std::string& ResultClass() const {
    DCHECK_EQ(RT_RESULT_RECORD, record_type());
    return primary_identifier();
  }

  // This is used for result-record and the async-output types and represents
  // their list of results.
  const std::vector<GdbRecordResult*>& results() const { return results_; }

 private:
  friend class GdbMiParser;

  explicit GdbRecord(RecordType record_type);
  GdbRecord(RecordType record_type, const std::string& token);

  void set_primary_identifier(const std::string& identifier) {
    primary_identifier_ = identifier;
  }

  // Takes ownership.
  void AddResult(GdbRecordResult* result) {
    results_.push_back(result);
  }

  RecordType record_type_;
  std::string token_;  // Optional string of digits corresponding with input.
  std::string primary_identifier_;

  std::vector<GdbRecordResult*> results_;

  DISALLOW_COPY_AND_ASSIGN(GdbRecord);
};

// Parse one line (record) of gdb/mi output. GdbMiReader is the top-level that
// reads one complete response.
class GdbMiParser {
 public:
  GdbMiParser();
  ~GdbMiParser();

  // Returns a parsed GdbRecord (one line of communication). Caller owns
  // returned value. |bytes_consumed|, if provided will be fill with how many
  // bytes of |input| were parsed.
  GdbRecord* Parse(const base::StringPiece& input, int* bytes_consumed);

 private:
  // Construct the top-level GdbRecord based on the initial character in the
  // input which determines the type. We also treat the '(gdb)' terminator as
  // pseudo-record of RT_TERMINATOR.
  GdbRecord* DetermineTypeAndMakeRecord();

  // If there's a leading set of digits, parse, advance past, and return them.
  std::string GetTokenIfAny();

  // Record that we encountered an error.
  void ReportError();

  // A lightweight check for whether there's at least |count| bytes of input
  // remaining in the input buffer.
  bool CanConsume(int count);

  // Parse, advance past, and return a C-style quoted string.
  std::string ConsumeString();

  // Parse, advance past, and return an identifier (see IsIdentifierChar for
  // definition 'identifier').
  std::string ConsumeIdentifier();

  // Parse, advance past, and return what the docs call a "result". That is,
  // an identifier followed by '=', followed by a rich "value" (below). Caller
  // owns the returned value.
  GdbRecordResult* ConsumeResult();

  // Parse, advance past, and return a "value", which can be either a
  // C-string, a tuple, or a list. Caller owns the returned value.
  base::Value* ConsumeValue();

  // Parse, advance past, and return a "tuple" (dictionary). This is a
  // {}-delimited, comma-separated of "result"s (above). Caller owns the
  // returned value.
  base::DictionaryValue* ConsumeTuple();

  // Parse, advance past, and return a "list". This is a []-delimited,
  // comma-separated list of either
  base::ListValue* ConsumeList();

  // Determine if the given character is in the identifier set.
  bool IsIdentifierChar(int c);

  // Advance past a 'nl' from the docs. Note that this is (strangely) either a
  // lone CR, or CR+LF (i.e. the opposite of what you would expect).
  void ConsumeNewline();

  // Parse and advance past the '(gdb)' terminator.
  void ConsumeTerminator();

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

class GdbOutput {
 public:
  GdbOutput();
  ~GdbOutput();

  size_t size() const { return records_.size(); }
  const GdbRecord* at(size_t i) const { return records_.at(i); }

 private:
  friend class GdbMiReader;

  std::vector<GdbRecord*> records_;
};

class GdbMiReader {
 public:
  GdbMiReader();
  ~GdbMiReader();

  // Reads a complete "output" response, and returns the parsed
  // representation. Caller owns. |bytes_consumed|, if provided will be fill
  // with how many bytes of |input| were parsed.
  GdbOutput* Parse(const base::StringPiece& input, int *bytes_consumed);

 private:
  GdbMiParser parser_;

  DISALLOW_COPY_AND_ASSIGN(GdbMiReader);
};


#endif  // SG_BACKEND_GDB_MI_PARSE_H_
