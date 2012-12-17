// Copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "sg/ui/tool_window_dragger.h"

#include "sg/render/renderer.h"
#include "sg/render/texture.h"
#include "sg/ui/docking_tool_window.h"
#include "sg/workspace.h"

namespace {

float kDetachedScale = 0.6f;
float kHoveringAlpha = 0.8f;

Texture kDockIndicatorTop;
Texture kDockIndicatorLeft;
Texture kDockIndicatorRight;
Texture kDockIndicatorBottom;

DropTargetIndicator IndicatorAt(Texture* texture, int x, int y) {
  DropTargetIndicator target;
  target.texture = texture;
  target.rect = Rect(x, y, texture->width, texture->height);
  return target;
}

void PlaceIndicatorsAroundEdge(
    const Rect& rect,
    std::vector<DropTargetIndicator>* into) {
  into->push_back(IndicatorAt(
      &kDockIndicatorTop,
      rect.x + rect.w / 2 - kDockIndicatorTop.width / 2,
      rect.y));
  into->push_back(IndicatorAt(
      &kDockIndicatorLeft,
      rect.x,
      rect.y + rect.h / 2 - kDockIndicatorLeft.height / 2));
  into->push_back(IndicatorAt(
      &kDockIndicatorRight,
      rect.x - kDockIndicatorRight.width,
      rect.y + rect.h / 2 - kDockIndicatorRight.height / 2));
  into->push_back(IndicatorAt(
      &kDockIndicatorBottom,
      rect.x + rect.w / 2 - kDockIndicatorBottom.width / 2,
      rect.y - kDockIndicatorBottom.height));
}

}  // namespace

ToolWindowDragger::ToolWindowDragger(
    DockingToolWindow* dragging,
    DragSetup* drag_setup)
    : dragging_(dragging),
      on_drop_target_(false),
      docking_workspace_(drag_setup->docking_workspace) {
  kDockIndicatorTop.name = L"art/dock-indicator-top.png";
  kDockIndicatorLeft.name = L"art/dock-indicator-left.png";
  kDockIndicatorRight.name = L"art/dock-indicator-right.png";
  kDockIndicatorBottom.name = L"art/dock-indicator-bottom.png";
  pick_up_offset_ = dragging_->ToClient(drag_setup->screen_position);
  initial_screen_rect_ = dragging_->GetScreenRect();
  current_position_ = drag_setup->screen_position;

  // TODO:
  // - Remove from tree
  // - Save target, split direction, fraction for cancel.
  // - Find all possible target split containers:
  //    All direct children of DockingSplitContainers
  // - Make a list of drag targets + icons of those plus root.

  Rect workspace_rect = docking_workspace_->GetScreenRect();
  PlaceIndicatorsAroundEdge(workspace_rect, &targets_);
}

ToolWindowDragger::~ToolWindowDragger() {
}

void ToolWindowDragger::Drag(const Point& screen_point) {
  // TODO:
  // - If over a drag target, figure out the space we would on an insert
  // -   SetScreenRect for target to that rect
  // -   Render at that rect, still alpha, but full size
  // - Otherwise,
  // -   Restore initial rect for detached draw
  // -   Render detached.
  current_position_ = screen_point;
}

void ToolWindowDragger::CancelDrag() {
  // TODO: Reinsert at "old" location. Save sibling, split direction, fraction.
}

void ToolWindowDragger::Render(Renderer* renderer) {
  // TODO(rendering): Hacky.
  if (!kDockIndicatorTop.data)
    renderer->LoadTexture(&kDockIndicatorTop);
  if (!kDockIndicatorLeft.data)
    renderer->LoadTexture(&kDockIndicatorLeft);
  if (!kDockIndicatorRight.data)
    renderer->LoadTexture(&kDockIndicatorRight);
  if (!kDockIndicatorBottom.data)
    renderer->LoadTexture(&kDockIndicatorBottom);

  // TODO(rendering): Not much practical reason to re-render this every frame
  // during drag. Investigate if it makes anything snappier if it's cached
  // after the first render.
  scoped_ptr<RenderToTextureRenderer> render_to_texture_renderer(
      renderer->CreateRenderToTextureRenderer(
          dragging_->GetScreenRect().w,
          dragging_->GetScreenRect().h));
  dragging_->Render(render_to_texture_renderer.get());

  for (size_t i = 0; i < targets_.size(); ++i) {
    const DropTargetIndicator& dti = targets_[i];
    renderer->DrawTexturedRect(dti.texture, dti.rect, 0, 0, 1, 1);
  }

  if (on_drop_target_) {
  } else {
    Point draw_at = current_position_.Subtract(
        pick_up_offset_.Scale(kDetachedScale));
    renderer->DrawRenderToTextureResult(
        render_to_texture_renderer.get(),
        Rect(draw_at.x, draw_at.y,
            static_cast<int>(dragging_->GetClientRect().w * kDetachedScale),
            static_cast<int>(dragging_->GetClientRect().h * kDetachedScale)),
        kHoveringAlpha,
        0.f, 0.f, 1.f, 1.f);
  }
}
