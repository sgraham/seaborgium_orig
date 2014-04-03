// Copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef SG_DISPLAY_UTIL_H_
#define SG_DISPLAY_UTIL_H_

#include "sg/base/string16.h"

string16 TidyTypeName(const string16& type);
string16 ToPlatformFileAndLine(const string16& filename, int line_number);

#endif  // SG_DISPLAY_UTIL_H_
