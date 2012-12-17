// Copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "sg/ui/tool_window_dragger.h"

#include "sg/render/renderer.h"
#include "sg/ui/docking_tool_window.h"

namespace {

float kDetachedScale = 0.7f;

}  // namespace

ToolWindowDragger::ToolWindowDragger(
    DockingToolWindow* dragging, const Point& screen_position)
    : dragging_(dragging) {
  // TODO: Remove from tree
  pick_up_offset_ = dragging_->ToClient(screen_position);
}

ToolWindowDragger::~ToolWindowDragger() {
}

void ToolWindowDragger::Drag(const Point& screen_point) {
  current_position_ = screen_point;
}

void ToolWindowDragger::CancelDrag() {
  // TODO: Reinsert at "old" location. Save sibling, split direction, fraction.
}

void ToolWindowDragger::Render(Renderer* renderer) {
  if (!render_to_texture_renderer_.get()) {
    render_to_texture_renderer_.reset(
        renderer->CreateRenderToTextureRenderer(
            dragging_->GetScreenRect().w,
            dragging_->GetScreenRect().h));
  }
  dragging_->Render(render_to_texture_renderer_.get());
  Point draw_at = current_position_.Subtract(
      pick_up_offset_.Scale(kDetachedScale));
  renderer->DrawRenderToTextureResult(
      render_to_texture_renderer_.get(),
      Rect(draw_at.x, draw_at.y,
           static_cast<int>(dragging_->GetClientRect().w * kDetachedScale),
           static_cast<int>(dragging_->GetClientRect().h * kDetachedScale)),
      0.7f,
      0.f, 0.f, 1.f, 1.f);
}
