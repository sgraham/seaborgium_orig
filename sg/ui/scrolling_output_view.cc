// Copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "sg/ui/scrolling_output_view.h"

#include "base/string_split.h"
#include "sg/render/renderer.h"
#include "sg/ui/skin.h"

ScrollingOutputView::ScrollingOutputView()
    : scroll_helper_(this, Skin::current().text_line_height()) {
}

ScrollingOutputView::~ScrollingOutputView() {
}

void ScrollingOutputView::Render(Renderer* renderer) {
  const Skin& skin = Skin::current();

  bool invalidate = scroll_helper_.Update();
  if (invalidate)
    Invalidate();

  renderer->SetDrawColor(skin.GetColorScheme().background());
  renderer->DrawFilledRect(GetClientRect());

  renderer->SetDrawColor(skin.GetColorScheme().text());
  int start_line = GetFirstLineInView();
  int line_height = skin.text_line_height();
  int y_pixel_scroll = scroll_helper_.GetOffset();
  for (size_t i = start_line; i < lines_.size(); ++i) {
    if (!LineInView(i))
      break;
    renderer->RenderText(
        skin.mono_font(),
        Point(0, i * line_height - y_pixel_scroll),
        lines_[i].c_str());
  }
  scroll_helper_.RenderScrollIndicators(renderer, skin);
}

void ScrollingOutputView::AddText(const string16& text) {
  std::vector<string16> result;
  base::SplitString(text, '\n', &result);
  for (size_t i = 0; i < result.size(); ++i) {
    // TODO(scottmg): Not sure if this is a great idea, but there seems to be
    // a bunch of blank lines from gdb, which is annoying.
    if (result[i].size() > 0)
      lines_.push_back(result[i]);
  }

  // TODO(scottmg): This should only happen when the scroll location is
  // already at "end".
  if (GetContentSize() > GetScreenRect().h)
    scroll_helper_.ScrollToEnd();
}

bool ScrollingOutputView::NotifyMouseMoved(
    int x, int y, int dx, int dy, const InputModifiers& modifiers) {
  return false;
}

bool ScrollingOutputView::NotifyMouseWheel(
    int delta, const InputModifiers& modifiers) {
  bool invalidate, handled;
  scroll_helper_.CommonMouseWheel(delta, modifiers, &invalidate, &handled);
  if (invalidate)
    Invalidate();
  return handled;
}

bool ScrollingOutputView::NotifyMouseButton(
    int index, bool down, const InputModifiers& modifiers) {
  return false;
}

bool ScrollingOutputView::NotifyKey(
    InputKey key, bool down, const InputModifiers& modifiers) {
  bool invalidate, handled;
  scroll_helper_.CommonNotifyKey(key, down, modifiers, &invalidate, &handled);
  if (invalidate)
    Invalidate();
  return handled;
}

int ScrollingOutputView::GetContentSize() {
  return Skin::current().text_line_height() * lines_.size();
}

int ScrollingOutputView::GetFirstLineInView() {
  return scroll_helper_.GetOffset() / Skin::current().text_line_height();
}

bool ScrollingOutputView::LineInView(int line_number) {
  int start_line = GetFirstLineInView();
  int line_height = Skin::current().text_line_height();
  if (line_number < start_line)
    return false;
  if ((line_number - start_line) * line_height > Height() + line_height)
    return false;
  return true;
}

