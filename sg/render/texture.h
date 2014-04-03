// Copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef SG_RENDER_TEXTURE_H_
#define SG_RENDER_TEXTURE_H_

#include "sg/base/string16.h"

class Renderer;

class Texture {
 public:
  Texture() : data(NULL), width(-1), height(-1), failed(false) {
  }

  void Load(const string16& name, Renderer* renderer);
  void Free(Renderer* renderer);

  string16 name;
  mutable void* data;
  mutable int width, height;
  mutable bool failed;
};

#endif  // SG_RENDER_TEXTURE_H_
