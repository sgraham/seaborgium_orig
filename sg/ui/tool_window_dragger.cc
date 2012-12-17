// Copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "sg/ui/tool_window_dragger.h"

#include "sg/render/renderer.h"
#include "sg/render/texture.h"
#include "sg/ui/docking_tool_window.h"
#include "sg/workspace.h"

namespace {

float kDetachedScale = 0.8f;
float kHoveringAlphaLow = 0.6f;
float kHoveringAlphaHigh = 0.8f;
float kDropTargetAlpha = 0.6f;

DropTargetIndicator IndicatorAt(
    Dockable* dockable, const Texture* texture, int x, int y) {
  DropTargetIndicator target;
  target.dockable = dockable;
  target.texture = texture;
  target.rect = Rect(x, y, texture->width, texture->height);
  return target;
}

void PlaceIndicatorsAroundEdge(
    const Rect& rect,
    std::vector<DropTargetIndicator>* into,
    Dockable* dockable) {
  const Skin& skin = Skin::current();
  into->push_back(IndicatorAt(
      dockable,
      skin.dock_top_texture(),
      rect.x + rect.w / 2 - skin.dock_top_texture()->width / 2,
      rect.y));
  into->push_back(IndicatorAt(
      dockable,
      skin.dock_left_texture(),
      rect.x,
      rect.y + rect.h / 2 - skin.dock_left_texture()->height / 2));
  into->push_back(IndicatorAt(
      dockable,
      skin.dock_right_texture(),
      rect.x + rect.w - skin.dock_right_texture()->width,
      rect.y + rect.h / 2 - skin.dock_right_texture()->height / 2));
  into->push_back(IndicatorAt(
      dockable,
      skin.dock_bottom_texture(),
      rect.x + rect.w / 2 - skin.dock_bottom_texture()->width / 2,
      rect.y + rect.h - skin.dock_bottom_texture()->height));
}

void PlaceIndicatorsAtCenter(
    const Rect& rect,
    std::vector<DropTargetIndicator>* into,
    Dockable* dockable) {
  const Skin& skin = Skin::current();
  int cx = rect.x + rect.w / 2;
  int cy = rect.y + rect.h / 2;
  into->push_back(IndicatorAt(
      dockable,
      skin.dock_top_texture(),
      cx - skin.dock_top_texture()->width / 2,
      cy - skin.dock_top_texture()->height * 2));
  into->push_back(IndicatorAt(
      dockable,
      skin.dock_left_texture(),
      cx - skin.dock_left_texture()->height * 2,
      cy - skin.dock_left_texture()->height / 2));
  into->push_back(IndicatorAt(
      dockable,
      skin.dock_right_texture(),
      cx + skin.dock_right_texture()->width,
      cy - skin.dock_right_texture()->height / 2));
  into->push_back(IndicatorAt(
      dockable,
      skin.dock_bottom_texture(),
      cx - skin.dock_bottom_texture()->width / 2,
      cy + skin.dock_bottom_texture()->height));
}

}  // namespace

ToolWindowDragger::ToolWindowDragger(
    DockingToolWindow* dragging,
    DragSetup* drag_setup)
    : dragging_(dragging),
      on_drop_target_(false),
      docking_workspace_(drag_setup->docking_workspace) {
  pick_up_offset_ = dragging_->ToClient(drag_setup->screen_position);
  initial_screen_rect_ = dragging_->GetScreenRect();
  current_position_ = drag_setup->screen_position;
  current_alpha_ = kHoveringAlphaLow;
  alpha_animate_ticks_ = 0;

  // TODO(drag):
  // - Remove from tree
  // - Save target, split direction, fraction for cancel.
  // - Find all possible target split containers:
  //    All direct children of DockingSplitContainers
  // - Make a list of drag targets + icons of those plus root.

  Rect workspace_rect = docking_workspace_->GetScreenRect();
  PlaceIndicatorsAroundEdge(
      workspace_rect, &targets_, docking_workspace_->GetRoot());
  std::vector<Dockable*> dock_targets =
      docking_workspace_->GetAllDockTargets();
  for (size_t i = 0; i < dock_targets.size(); ++i) {
    if (dock_targets[i] != dragging_) {
      PlaceIndicatorsAtCenter(
          dock_targets[i]->GetScreenRect(), &targets_, dock_targets[i]);
    }
  }
}

ToolWindowDragger::~ToolWindowDragger() {
}

void ToolWindowDragger::Drag(const Point& screen_point) {
  // TODO(drag):
  // - If over a drag target, figure out the space we would on an insert
  // -   SetScreenRect for target to that rect
  // -   Render at that rect, still alpha, but full size
  // - Otherwise,
  // -   Restore initial rect for detached draw
  // -   Render detached.
  current_position_ = screen_point;
}

void ToolWindowDragger::CancelDrag() {
  // TODO(drag):
  // Reinsert at "old" location. Save sibling, split direction, fraction.
}

void ToolWindowDragger::Render(Renderer* renderer) {
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
    renderer->DrawTexturedRectAlpha(
        dti.texture, dti.rect, kDropTargetAlpha, 0, 0, 1, 1);
  }

  // TODO(rendering): Another update/render.
  if ((alpha_animate_ticks_++ / 30) % 2 == 0)
    current_alpha_ += (kHoveringAlphaHigh - current_alpha_) * 0.05f;
  else
    current_alpha_ += (kHoveringAlphaLow - current_alpha_) * 0.05f;

  if (on_drop_target_) {
  } else {
    Point draw_at = current_position_.Subtract(
        pick_up_offset_.Scale(kDetachedScale));
    Rect dest(draw_at.x, draw_at.y,
              static_cast<int>(dragging_->GetClientRect().w * kDetachedScale),
              static_cast<int>(dragging_->GetClientRect().h * kDetachedScale));
    renderer->DrawRenderToTextureResult(
        render_to_texture_renderer.get(),
        dest,
        current_alpha_,
        0.f, 0.f, 1.f, 1.f);
    renderer->SetDrawColor(Color(0, 128, 128, current_alpha_ * 128));
    renderer->DrawFilledRect(dest);
  }

  Workspace::Invalidate();
}
