// Copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "sg/display_util.h"

#include <string>

#include "base/utf_string_conversions.h"
#include "re2/re2.h"

string16 TidyTypeName(const string16& type) {
  // Change "char *" to "char*"
  // TODO(scottmg): References.
  if (type.find(L"*") == string16::npos)  // Early out to avoid unnecessary RE.
    return type;
  std::string utf8 = UTF16ToUTF8(type);
  if (RE2::Replace(&utf8, "(.*?)\\s+(\\*+)", "\\1\\2"))
    return UTF8ToUTF16(utf8);
  return type;
}
