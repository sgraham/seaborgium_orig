// Copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "sg/ui/docking_tool_window.h"

#include "sg/render/font.h"
#include "sg/render/renderer.h"
#include "sg/render/scoped_render_offset.h"
#include "sg/ui/skin.h"

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

void DockingToolWindow::Render(Renderer* renderer, const Skin& skin) {
  renderer->SetDrawColor(skin.GetColorScheme().title_bar_inactive());
  renderer->DrawFilledRect(Rect(0, 0, Width(), skin.title_bar_size()));
  renderer->SetDrawColor(skin.GetColorScheme().title_bar_text_inactive());
  renderer->RenderText(&kUIFont, Point(kTitleOffset, 0), title_);
  //renderer->RenderChild(base::Bind(Dockabe
  ScopedRenderOffset offset(renderer, this, contents_);
  contents_->Render(renderer, skin);
}

void DockingToolWindow::SetScreenRect(const Rect& rect) {
  Dockable::SetScreenRect(rect);
  Rect contents_rect = rect;
  // F.
  //rect.y += skin.title_bar_size();
  contents_rect.y += 19;
  contents_rect.h -= 19;
  contents_->SetScreenRect(contents_rect);
}
