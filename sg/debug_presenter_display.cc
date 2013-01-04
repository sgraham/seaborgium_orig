// Copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "sg/debug_presenter_display.h"

DebugPresenterVariable::DebugPresenterVariable() {}

DebugPresenterVariable::DebugPresenterVariable(
    const string16& type, const string16& name)
    : type_(type),
      name_(name) {
  key_ = type + L"\t" + name;
}

DebugPresenterDisplay::~DebugPresenterDisplay() {
}
