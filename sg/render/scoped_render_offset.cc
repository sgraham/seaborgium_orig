// Copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "sg/render/scoped_render_offset.h"

#include "sg/render/renderer.h"
#include "sg/ui/dockable.h"

ScopedRenderOffset::ScopedRenderOffset(
    Renderer* renderer, Dockable* parent, Dockable* child)
    : renderer_(renderer) {
  old_offset_ = renderer_->GetRenderOffset();
  Rect relative = child->GetScreenRect().RelativeTo(parent->GetScreenRect());
  renderer_->AddRenderOffset(Point(relative.x, relative.y));
}

ScopedRenderOffset::ScopedRenderOffset(Renderer* renderer, int dx, int dy)
    : renderer_(renderer) {
  old_offset_ = renderer_->GetRenderOffset();
  renderer_->AddRenderOffset(Point(dx, dy));
}

ScopedRenderOffset::~ScopedRenderOffset() {
  renderer_->SetRenderOffset(old_offset_);
}
