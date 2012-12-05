// Copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef SG_BACKEND_BACKEND_NATIVE_WIN_H_
#define SG_BACKEND_BACKEND_NATIVE_WIN_H_

#include "base/string16.h"

// Helpers shared amongst the native Windows backend.

string16 FormatLastError(const string16& function);

#endif  // SG_BACKEND_BACKEND_NATIVE_WIN_H_
