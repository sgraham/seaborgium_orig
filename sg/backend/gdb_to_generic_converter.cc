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

const base::ListValue* FindListValue(
    const std::string& key,
    const std::vector<GdbRecordResult*>& results) {
  for (size_t i = 0; i < results.size(); ++i) {
    if (results[i]->variable() == key) {
      const ListValue* result;
      if (results[i]->value()->GetAsList(&result))
        return result;
    }
  }
  NOTREACHED();
  return NULL;
}

FrameData FrameDataFromDictionaryValue(const base::DictionaryValue* dict) {
  std::string addr_string, filename_string, function_string, line_string;
  CHECK(dict->GetStringWithoutPathExpansion("addr", &addr_string));
  CHECK(dict->GetStringWithoutPathExpansion("func", &function_string));
  // file and line may not be available if we have no symbols.
  dict->GetStringWithoutPathExpansion("file", &filename_string);
  dict->GetStringWithoutPathExpansion("line", &line_string);
  FrameData data;
  CHECK(addr_string[0] == '0' && addr_string[1] == 'x');
  int temp;
  // TODO(scottmg): Need HexStringToUint64.
  CHECK(base::HexStringToInt(addr_string.substr(2), &temp));
  data.address = static_cast<uintptr_t>(temp);
  data.function = UTF8ToUTF16(function_string);
  data.filename = UTF8ToUTF16(filename_string);
  data.line_number = 0;
  base::StringToInt(line_string, &data.line_number);
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

TypeNameValue DictionaryToTypeNameValue(base::DictionaryValue* dict_value) {
  TypeNameValue data;
  CHECK(dict_value->GetStringWithoutPathExpansion("name", &data.name));
  CHECK(dict_value->GetStringWithoutPathExpansion("type", &data.type));
  CHECK(dict_value->GetStringWithoutPathExpansion("value", &data.value));
  return data;
}

RetrievedStackData MergeArgumentsIntoStackFrameData(
    const RetrievedStackData& just_stack,
    base::Value* value) {
  base::ListValue* list_value;
  CHECK(value->GetAsList(&list_value));
  if (list_value->GetSize() != just_stack.frames.size()) {
    // Don't match, bail.
    return just_stack;
  }
  RetrievedStackData data = just_stack;
  for (size_t i = 0; i < list_value->GetSize(); ++i) {
    base::DictionaryValue* dict_value;
    CHECK(list_value->GetDictionary(i, &dict_value));
    base::DictionaryValue* frame_dict_value;
    CHECK(dict_value->GetDictionary("frame", &frame_dict_value));
    base::ListValue* args_list_value;
    CHECK(frame_dict_value->GetList("args", &args_list_value));
    std::vector<TypeNameValue>* arguments = &data.frames[i].arguments;
    for (size_t j = 0; j < args_list_value->GetSize(); ++j) {
      base::DictionaryValue* argument_dictionary;
      CHECK(args_list_value->GetDictionary(j, &argument_dictionary));
      arguments->push_back(DictionaryToTypeNameValue(argument_dictionary));
    }
  }
  return data;
}

RetrievedLocalsData RetrievedLocalsDataFromList(base::Value* value) {
  base::ListValue* list_value;
  CHECK(value->GetAsList(&list_value));
  RetrievedLocalsData data;
  for (size_t i = 0; i < list_value->GetSize(); ++i) {
    base::DictionaryValue* dict_value;
    CHECK(list_value->GetDictionary(i, &dict_value));
    string16 name;
    CHECK(dict_value->GetStringWithoutPathExpansion("name", &name));
    data.local_names.push_back(name);
  }
  return data;
}

LibraryLoadedData LibraryLoadedDataFromRecordResults(
    const std::vector<GdbRecordResult*>& results) {
  LibraryLoadedData data;
  data.target_path = UTF8ToUTF16(FindStringValue("target-name", results));
  data.host_path = UTF8ToUTF16(FindStringValue("host-name", results));
  data.thread_id = UTF8ToUTF16(FindStringValue("thread-group", results));
  std::string symbols_string = FindStringValue("symbols-loaded", results);
  int symbols_int;
  CHECK(base::StringToInt(symbols_string, &symbols_int));
  data.symbols_loaded = symbols_int;
  return data;
}

WatchCreatedData WatchCreatedDataFromRecordResults(
    const std::vector<GdbRecordResult*>& results) {
  WatchCreatedData data;
  data.variable_id = FindStringValue("name", results);
  std::string numchild_str = FindStringValue("numchild", results);
  std::string has_more_str = FindStringValue("has_more", results);
  data.has_children = false;
  int numchild_int, has_more_int;
  if (base::StringToInt(numchild_str, &numchild_int) &&
      base::StringToInt(has_more_str, &has_more_int) &&
      (numchild_int > 0 || has_more_int != 0)) {
    data.has_children = true;
  }
  data.value = UTF8ToUTF16(FindStringValue("value", results));
  data.type = UTF8ToUTF16(FindStringValue("type", results));
  return data;
}

WatchesUpdatedData WatchesUpdatedDataFromChangesList(base::Value* value) {
  WatchesUpdatedData data;
  base::ListValue* list_value;
  CHECK(value->GetAsList(&list_value));
  for (size_t i = 0; i < list_value->GetSize(); ++i) {
    base::DictionaryValue* dict_value;
    CHECK(list_value->GetDictionary(i, &dict_value));
    WatchesUpdatedData::Item item;
    string16 variable_id;
    CHECK(dict_value->GetStringWithoutPathExpansion("name", &variable_id));
    item.variable_id = UTF16ToUTF8(variable_id);
    dict_value->GetStringWithoutPathExpansion("value", &item.value);
    string16 type_changed;
    CHECK(dict_value->GetStringWithoutPathExpansion(
        "type_changed", &type_changed));
    item.type_changed = type_changed == L"true";
    data.watches.push_back(item);
  }
  return data;
}

WatchesChildListData WatchesChildListDataFromRecordResults(
    const std::vector<GdbRecordResult*>& results) {
  WatchesChildListData data;
  std::string numchild = FindStringValue("numchild", results);
  if (numchild == "0")
    return data;
  const base::ListValue* children = FindListValue("children", results);
  for (size_t i = 0; i < children->GetSize(); ++i) {
    WatchesChildListData::Child child;
    const base::DictionaryValue* dict_value;
    const base::DictionaryValue* child_dict;
    CHECK(children->GetDictionary(i, &dict_value));
    CHECK(dict_value->GetDictionaryWithoutPathExpansion("child", &child_dict));
    CHECK(
        child_dict->GetStringWithoutPathExpansion("name", &child.variable_id));
    CHECK(child_dict->GetStringWithoutPathExpansion("exp", &child.expression));
    // No value for complex types.
    child_dict->GetStringWithoutPathExpansion("value", &child.value);
    // No type for stupid public/private pseudo members.
    // TODO(scottmg): Figure out how to (at least normally) hide public, etc.
    child_dict->GetStringWithoutPathExpansion("type", &child.type);
    string16 numchild_str;
    CHECK(child_dict->GetStringWithoutPathExpansion("numchild", &numchild_str));
    int numchild_int;  // TODO(gdb-python): has_more?
    child.has_children =
        base::StringToInt(numchild_str, &numchild_int) && numchild_int > 0;
    data.children.push_back(child);
  }
  return data;
}
