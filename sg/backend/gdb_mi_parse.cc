// Copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "sg/backend/gdb_mi_parse.h"

#include <ctype.h>

#include "base/logging.h"
#include "base/utf_string_conversions.h"
#include "base/values.h"

namespace {
const int32 kExtendedASCIIStart = 0x80;
}  // namespace

GdbRecordResult::GdbRecordResult() {
}

GdbRecordResult::~GdbRecordResult() {
}

GdbRecord::GdbRecord(RecordType record_type) : record_type_(record_type) {
}

GdbRecord::GdbRecord(RecordType record_type, const std::string& token)
    : record_type_(record_type),
      token_(token) {
}

GdbRecord::~GdbRecord() {
  for (size_t i = 0; i < results_.size(); ++i)
    delete results_[i];
}

GdbMiParser::GdbMiParser()
    : start_pos_(NULL),
      pos_(NULL),
      end_pos_(NULL),
      error_(false),
      error_index_(0) {
}

GdbMiParser::~GdbMiParser() {
}

GdbRecord* GdbMiParser::Parse(
    const base::StringPiece& input, int* bytes_consumed) {
  start_pos_ = input.data();
  pos_ = start_pos_;
  end_pos_ = start_pos_ + input.length();
  error_ = false;
  error_index_ = 0;

  scoped_ptr<GdbRecord> record(DetermineTypeAndMakeRecord());
  if (error_)
    return NULL;

  switch (record->record_type()) {
    case GdbRecord::RT_CONSOLE_STREAM_OUTPUT:
    case GdbRecord::RT_TARGET_STREAM_OUTPUT:
    case GdbRecord::RT_LOG_STREAM_OUTPUT:
      record->set_primary_identifier(ConsumeString());
      break;
    case GdbRecord::RT_EXEC_ASYNC_OUTPUT:
    case GdbRecord::RT_STATUS_ASYNC_OUTPUT:
    case GdbRecord::RT_NOTIFY_ASYNC_OUTPUT:
    case GdbRecord::RT_RESULT_RECORD:
      record->set_primary_identifier(ConsumeIdentifier());
      while (CanConsume(1) && *pos_ == ',' && !error_) {
        ++pos_;
        record->AddResult(ConsumeResult());
      }
      break;
    case GdbRecord::RT_TERMINATOR:
      ConsumeTerminator();
      break;
    default:
      ReportError();
  }

  // The stream output records aren't documented as having a trailing "nl", but
  // clearly do.
  ConsumeNewline();

  // We don't test for extra input at the end here so that we can be used as a
  // subparser in the full output parser.

  if (error_)
    return NULL;

  if (bytes_consumed)
    *bytes_consumed = pos_ - start_pos_;
  return record.release();
}

GdbRecord* GdbMiParser::DetermineTypeAndMakeRecord() {
  std::string token = GetTokenIfAny();

  if (!CanConsume(1)) {
    ReportError();
    return NULL;
  }

  switch (*pos_++) {
    case '^':
      return new GdbRecord(GdbRecord::RT_RESULT_RECORD, token);
    case '~':
      CHECK_EQ(0, token.size());
      return new GdbRecord(GdbRecord::RT_CONSOLE_STREAM_OUTPUT);
    case '@':
      CHECK_EQ(0, token.size());
      return new GdbRecord(GdbRecord::RT_TARGET_STREAM_OUTPUT);
    case '&':
      CHECK_EQ(0, token.size());
      return new GdbRecord(GdbRecord::RT_LOG_STREAM_OUTPUT);
    case '*':
      return new GdbRecord(GdbRecord::RT_EXEC_ASYNC_OUTPUT, token);
    case '+':
      return new GdbRecord(GdbRecord::RT_STATUS_ASYNC_OUTPUT, token);
    case '=':
      return new GdbRecord(GdbRecord::RT_NOTIFY_ASYNC_OUTPUT, token);
    case '(':
      CHECK_EQ(0, token.size());
      return new GdbRecord(GdbRecord::RT_TERMINATOR, token);
    default:
      ReportError();
      return NULL;
  }
}

std::string GdbMiParser::GetTokenIfAny() {
  std::string result;
  for (;;) {
    if (!CanConsume(1)) {
      ReportError();
      return result;
    }
    switch (*pos_) {
      case '0':
      case '1':
      case '2':
      case '3':
      case '4':
      case '5':
      case '6':
      case '7':
      case '8':
      case '9':
        result += *pos_++;
        break;
      default:
        return result;
    }
  }
}

void GdbMiParser::ReportError() {
  // Note the first failed location only.
  if (!error_) {
    error_ = true;
    error_index_ = pos_ - start_pos_;
  }
}

bool GdbMiParser::CanConsume(int length) {
  return pos_ + length <= end_pos_;
}

std::string GdbMiParser::ConsumeString() {
  if (!CanConsume(1) || *pos_ != '"') {
    ReportError();
    return std::string();
  }
  ++pos_;

  std::string result;
  int next_char;

  while (CanConsume(1)) {
    next_char = *pos_++;
    if (next_char == '\\') {
      if (!CanConsume(1)) {
        ReportError();
        return std::string();
      }
      switch (*pos_++) {
        // TODO(scottmg): x or u?
        case '"':
          result.push_back('"');
          break;
        case '\\':
          result.push_back('\\');
          break;
        case '/':
          result.push_back('/');
          break;
        case 'b':
          result.push_back('\b');
          break;
        case 'f':
          result.push_back('\f');
          break;
        case 'n':
          result.push_back('\n');
          break;
        case 'r':
          result.push_back('\r');
          break;
        case 't':
          result.push_back('\t');
          break;
        case 'v':
          result.push_back('\v');
          break;
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9': {
          // Not sure what this is. I assume octal, and always 3 digits.
          int digit = (*(pos_ - 1) * 64) + *(pos_) * 8 + *(pos_ + 1);
          pos_ += 2;
          string16 wide;
          wide.push_back(digit);
          std::string as_utf8(UTF16ToUTF8(wide));
          for (size_t i = 0; i < as_utf8.size(); ++i)
            result.push_back(as_utf8[i]);
          break;
        }
        default:
          ReportError();
          return std::string();
      }
    } else if (next_char == '"') {
      return result;
    } else {
      if (next_char <= kExtendedASCIIStart) {
        result.push_back(static_cast<char>(next_char));
      } else {
        ReportError();
        return std::string();
      }
    }
  }
  ReportError();
  return std::string();
}

std::string GdbMiParser::ConsumeIdentifier() {
  std::string result;
  while (CanConsume(1)) {
    int next_char = *pos_++;
    if (IsIdentifierChar(next_char)) {
      result.push_back(static_cast<char>(next_char));
    } else {
      --pos_;
      return result;
    }
  }
  ReportError();
  return std::string();
}

GdbRecordResult* GdbMiParser::ConsumeResult() {
  scoped_ptr<GdbRecordResult> result(new GdbRecordResult);
  result->set_variable(ConsumeIdentifier());
  if (error_ || !CanConsume(1) || *pos_++ != '=') {
    ReportError();
    return NULL;
  }
  result->set_value(ConsumeValue());
  if (error_)
    return NULL;
  return result.release();
}

base::Value* GdbMiParser::ConsumeValue() {
  if (!CanConsume(1)) {
    ReportError();
    return NULL;
  }

  switch (*pos_) {
    case '"':
      return new base::StringValue(ConsumeString());
    case '{':
      return ConsumeTuple();
    case '[':
      return ConsumeList();
    default:
      ReportError();
      return NULL;
  }
}

base::DictionaryValue* GdbMiParser::ConsumeTuple() {
  DCHECK(CanConsume(1));
  DCHECK_EQ('{', *pos_);  // This is verified in ConsumeValue.
  pos_++;
  scoped_ptr<base::DictionaryValue> result(new base::DictionaryValue);
  while (CanConsume(1)) {
    if (*pos_ == '}') {
      ++pos_;
      return result.release();
    }
    scoped_ptr<GdbRecordResult> sub_result(ConsumeResult());
    if (error_)
      break;
    result->SetWithoutPathExpansion(
        sub_result->variable(), sub_result->release_value());
    if (*pos_ == ',')  // This is strictly unnecessary for parsing, so skip it.
      ++pos_;
  }
  return NULL;
}

base::ListValue* GdbMiParser::ConsumeList() {
  DCHECK(CanConsume(1));
  DCHECK_EQ('[', *pos_);  // This is verified in ConsumeValue.
  pos_++;
  scoped_ptr<base::ListValue> result(new base::ListValue);
  while (CanConsume(1)) {
    if (*pos_ == ']') {
      ++pos_;
      return result.release();
    }
    // Can either be a list of "value" or a list of "result". This parse is
    // slightly more loose than the docs in that we allow heterogeneous.
    if (IsIdentifierChar(*pos_)) {
      // We know it must be an identifier, so it's the beginning of an
      // "identifier=value".
      scoped_ptr<GdbRecordResult> sub_result(ConsumeResult());
      // TODO(scottmg): Possibly get rid of GdbRecordResult and make it an
      // untyped DictionaryValue?
      scoped_ptr<base::DictionaryValue> converted(new base::DictionaryValue);
      converted->Set(sub_result->variable(), sub_result->release_value());
      result->Append(converted.release());
    } else {
      scoped_ptr<Value> value(ConsumeValue());
      if (error_)
        break;
      result->Append(value.release());
    }
    if (*pos_ == ',')  // This is strictly unnecessary for parsing, so skip it.
      ++pos_;
  }
  return NULL;
}

bool GdbMiParser::IsIdentifierChar(int c) {
  return c == '_' || c == '-' || isalpha(c);
}

void GdbMiParser::ConsumeNewline() {
  if (!CanConsume(1)) {
    ReportError();
    return;
  }
  if (*pos_++ == '\r') {
    if (CanConsume(1)) {
      if (*pos_ == '\n')
        ++pos_;
    }
  } else {
    ReportError();
  }
}

void GdbMiParser::ConsumeTerminator() {
  // Note that we treat the value as the remainder: "gdb)" to make handling
  // the same as the other record types that have a leading indictor.
  if (!CanConsume(5) ||
      pos_[0] != 'g' ||
      pos_[1] != 'd' ||
      pos_[2] != 'b' ||
      pos_[3] != ')' ||
      pos_[4] != ' ') {
    ReportError();
    return;
  }
  pos_ += 5;
}

GdbOutput::GdbOutput() {
}

GdbOutput::~GdbOutput() {
  for (size_t i = 0; i < records_.size(); ++i)
    delete records_[i];
}

GdbMiReader::GdbMiReader() {
}

GdbMiReader::~GdbMiReader() {
}

GdbOutput* GdbMiReader::Parse(
    const base::StringPiece& input, int* bytes_consumed) {
  base::StringPiece remaining(input);
  if (remaining[0] == '\n') {
    // See test GdbMiParse.IncrementalWithMultibyteLineEnding.
    remaining = base::StringPiece(remaining.data() + 1, remaining.size() - 1);
  }
  scoped_ptr<GdbOutput> result(new GdbOutput);
  for (;;) {
    int consumed = 0;
    scoped_ptr<GdbRecord> record(parser_.Parse(remaining, &consumed));
    if (!record.get()) {
      if (bytes_consumed)
        *bytes_consumed = 0;
      return NULL;
    }
    remaining.set(remaining.data() + consumed, remaining.size() - consumed);
    if (record->record_type() == GdbRecord::RT_TERMINATOR) {
      if (bytes_consumed)
        *bytes_consumed = remaining.data() - input.data();
      return result.release();
    }
    result->records_.push_back(record.release());
  }
  return NULL;
}
