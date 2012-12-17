// Copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "sg/ui/docking_tool_window.h"

#include "sg/render/font.h"
#include "sg/render/renderer.h"
#include "sg/render/scoped_render_offset.h"
#include "sg/ui/draggable.h"
#include "sg/ui/skin.h"
#include "sg/ui/tool_window_dragger.h"

namespace {

const int kTitleOffset = 3;
Font kUIFont(L"Segoe UI", 12.f);

}

DockingToolWindow::DockingToolWindow(Dockable* contents, const string16& title)
    : contents_(contents),
      title_(title) {
}

DockingToolWindow::~DockingToolWindow() {
}

Rect DockingToolWindow::RectForTitleBar() {
  return Rect(0, 0, Width(), Skin::current().title_bar_size());
}

void DockingToolWindow::Render(Renderer* renderer) {
  const Skin& skin = Skin::current();
  renderer->SetDrawColor(skin.GetColorScheme().title_bar_inactive());
  renderer->DrawFilledRect(RectForTitleBar());
  renderer->SetDrawColor(skin.GetColorScheme().title_bar_text_inactive());
  renderer->RenderText(&kUIFont, Point(kTitleOffset, 0), title_);
  ScopedRenderOffset offset(renderer, this, contents_);
  contents_->Render(renderer);
}

void DockingToolWindow::SetScreenRect(const Rect& rect) {
  Dockable::SetScreenRect(rect);
  Rect contents_rect = rect;
  const Skin& skin = Skin::current();
  contents_rect.y += skin.title_bar_size();
  contents_rect.h -= skin.title_bar_size();
  contents_->SetScreenRect(contents_rect);
}

bool DockingToolWindow::CouldStartDrag(
      const Point& screen_position,
      DragDirection* direction,
      scoped_ptr<Draggable>* draggable) {
  if (RectForTitleBar().Contains(ToClient(screen_position))) {
    *direction = kDragDirectionAll;
    if (draggable)
      draggable->reset(new ToolWindowDragger(this, screen_position));
    return true;
  }
  return false;
}
