// Copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef SG_RENDER_FONT_H_
#define SG_RENDER_FONT_H_

#include "base/string16.h"

class Font {
 public:
  Font() : data(NULL), size(-1), bold(false) {
  }
  Font(const string16& name, float size)
      : data(NULL), facename(name), size(size), bold(false) {
  }

  string16 facename;
  float size;
  mutable void* data;
  bool bold;
};

#endif  // SG_RENDER_FONT_H_
