// Copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "sg/backend/gdb_mi_parse.h"

#include "base/logging.h"
#include "base/values.h"

namespace {

const int32 kExtendedASCIIStart = 0x80;

}  // namespace

GdbRecord::GdbRecord(RecordType record_type) : record_type_(record_type) {
}

GdbRecord::GdbRecord(RecordType record_type, const std::string& token)
    : record_type_(record_type),
      token_(token) {
}

GdbRecord::~GdbRecord() {
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

GdbRecord* GdbMiParser::Parse(const base::StringPiece& input) {
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
    default:
      NOTREACHED() << "todo;";
  }

  if (error_)
    return NULL;
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
      CHECK(token.size() == 0);
      return new GdbRecord(GdbRecord::RT_CONSOLE_STREAM_OUTPUT);
    case '@':
      CHECK(token.size() == 0);
      return new GdbRecord(GdbRecord::RT_TARGET_STREAM_OUTPUT);
    case '&':
      CHECK(token.size() == 0);
      return new GdbRecord(GdbRecord::RT_LOG_STREAM_OUTPUT);
    case '*':
      return new GdbRecord(GdbRecord::RT_EXEC_ASYNC_OUTPUT, token);
    case '+':
      return new GdbRecord(GdbRecord::RT_STATUS_ASYNC_OUTPUT, token);
    case '=':
      return new GdbRecord(GdbRecord::RT_NOTIFY_ASYNC_OUTPUT, token);
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
  char next_char = 0;

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
        default:
          ReportError();
          return std::string();
      }
    } else if (next_char == '"') {
      return result;
    } else {
      if (next_char <= kExtendedASCIIStart) {
        result.push_back(next_char);
      } else {
        ReportError();
        return std::string();
      }
    }
  }
  ReportError();
  return std::string();
}
