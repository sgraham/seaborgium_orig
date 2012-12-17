// Copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef SG_RENDER_SCOPED_RENDER_OFFSET_H_
#define SG_RENDER_SCOPED_RENDER_OFFSET_H_

#include "sg/basic_geometric_types.h"

class Renderer;
class Dockable;

class ScopedRenderOffset {
 public:
  ScopedRenderOffset(Renderer* renderer, Dockable* parent, Dockable* child);
  ~ScopedRenderOffset();

 private:
  Renderer* renderer_;
  Point old_offset_;
};

#endif  // SG_RENDER_SCOPED_RENDER_OFFSET_H_
