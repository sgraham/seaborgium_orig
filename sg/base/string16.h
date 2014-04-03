// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef SG_BASE_STRING16_H_
#define SG_BASE_STRING16_H_

#include <string>

#include "build/build_config.h"

#if defined(WCHAR_T_IS_UTF16)

typedef wchar_t char16;
typedef std::wstring string16;
typedef std::char_traits<wchar_t> string16_char_traits;

#elif defined(WCHAR_T_IS_UTF32)

#error TODO

#endif

#endif  // SG_BASE_STRING16_H_
