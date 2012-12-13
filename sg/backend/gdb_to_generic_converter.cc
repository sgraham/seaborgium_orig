// Copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "sg/backend/gdb_to_generic_converter.h"

#include "base/string_number_conversions.h"
#include "base/utf_string_conversions.h"

std::string FindStringValue(
    const std::string& key,
    const std::vector<GdbRecordResult*>& results) {
  for (size_t i = 0; i < results.size(); ++i) {
    if (results[i]->variable() == key) {
      std::string result;
      if (results[i]->value()->GetAsString(&result))
        return result;
    }
  }
  NOTREACHED();
  return NULL;
}

const base::DictionaryValue* FindDictionaryValue(
    const std::string& key,
    const std::vector<GdbRecordResult*>& results) {
  for (size_t i = 0; i < results.size(); ++i) {
    if (results[i]->variable() == key) {
      const DictionaryValue* result;
      if (results[i]->value()->GetAsDictionary(&result))
        return result;
    }
  }
  NOTREACHED();
  return NULL;
}

FrameData FrameDataFromDictionaryValue(const base::DictionaryValue* dict) {
  std::string addr_string, filename_string, function_string, line_string;
  CHECK(dict->GetStringWithoutPathExpansion("addr", &addr_string));
  CHECK(dict->GetStringWithoutPathExpansion("file", &filename_string));
  CHECK(dict->GetStringWithoutPathExpansion("func", &function_string));
  CHECK(dict->GetStringWithoutPathExpansion("line", &line_string));
  FrameData data;
  CHECK(addr_string[0] == '0' && addr_string[1] == 'x');
  int temp;
  // TODO(scottmg): Need HexStringToUint64.
  CHECK(base::HexStringToInt(addr_string.substr(2), &temp));
  data.address = static_cast<uintptr_t>(temp);
  data.function = UTF8ToUTF16(function_string);
  data.filename = UTF8ToUTF16(filename_string);
  CHECK(base::StringToInt(line_string, &data.line_number));
  return data;
}

StoppedAtBreakpointData StoppedAtBreakpointDataFromRecordResults(
    const std::vector<GdbRecordResult*>& results) {
  StoppedAtBreakpointData data;
  data.frame = FrameDataFromDictionaryValue(
      FindDictionaryValue("frame", results));
  return data;
}

StoppedAfterSteppingData StoppedAfterSteppingDataFromRecordResults(
    const std::vector<GdbRecordResult*>& results) {
  StoppedAfterSteppingData data;
  data.frame = FrameDataFromDictionaryValue(
      FindDictionaryValue("frame", results));
  return data;
}

RetrievedStackData RetrievedStackDataFromList(base::Value* value) {
  base::ListValue* list_value;
  CHECK(value->GetAsList(&list_value));
  RetrievedStackData data;
  for (size_t i = 0; i < list_value->GetSize(); ++i) {
    base::DictionaryValue* dict_value;
    CHECK(list_value->GetDictionary(i, &dict_value));
    base::DictionaryValue* frame_dict_value;
    CHECK(dict_value->GetDictionary("frame", &frame_dict_value));
    data.frames.push_back(FrameDataFromDictionaryValue(frame_dict_value));
  }
  return data;
}
