// Copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "sg/backend/backend_native_win.h"

#include <windows.h>

string16 FormatLastError(const string16& function) {
  LPTSTR error_text = NULL;
  FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM |
                    FORMAT_MESSAGE_ALLOCATE_BUFFER |
                    FORMAT_MESSAGE_IGNORE_INSERTS,
                NULL, GetLastError(),
                MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                (LPTSTR)&error_text, 0, NULL);
  if (error_text) {
    string16 result(function + string16(L": "));
    result += error_text;
    LocalFree(error_text);
    return result;
  } else {
    return string16(function + L": couldn't retrieve error!");
  }
}
