// Copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef SG_RENDER_TEXTURE_H_
#define SG_RENDER_TEXTURE_H_

#include "base/string16.h"

namespace Renderer { class Base; }

class Texture {
 public:
  Texture() : data(NULL), width(-1), height(-1), failed(false) {
  }

  void Load(const string16& name, Renderer::Base* renderer);
  void Free(Renderer::Base* renderer);

  string16 name;
  void* data;
  int width, height;
  bool failed;
};

#endif  // SG_RENDER_TEXTURE_H_
