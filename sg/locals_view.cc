// Copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "sg/locals_view.h"

#include "base/logging.h"
#include "sg/ui/skin.h"

LocalsView::LocalsView(const Skin& skin) : Contents(skin) {
  // TODO(rendering): Share with source view.
  font_.facename = L"Consolas";
  font_.size = 13.f;
}

void LocalsView::SetData(const std::vector<TypeNameValue>& locals) {
  lines_.clear();
  Invalidate();
}

void LocalsView::Render(Renderer* renderer) {
  /*
  const Skin& skin = Contents::GetSkin();
  int line_height = skin.text_line_height();

  // TODO(rendering): Hacky.
  if (!g_pc_indicator_texture.data) {
    renderer->LoadTexture(&g_pc_indicator_texture);
  }

  renderer->SetDrawColor(skin.GetColorScheme().background());
  renderer->DrawFilledRect(Rect(0, 0, Width(), Height()));

  static const int left_margin = 5;
  static const int right_margin = 5;
  static const int indicator_width = line_height;
  static const int indicator_height = line_height;
  static const int full_margin_width =
      left_margin + indicator_width + right_margin;

  renderer->SetDrawColor(skin.GetColorScheme().margin());
  renderer->DrawFilledRect(Rect(0, 0, full_margin_width, Height()));

  // TODO(rendering): Generic scroll pane.
  renderer->SetDrawColor(skin.GetColorScheme().text());
  for (size_t i = 0; i < lines_.size(); ++i) {
    renderer->RenderText(
        &font_, Point(full_margin_width, i * line_height), lines_[i]);
  }

  renderer->SetDrawColor(skin.GetColorScheme().pc_indicator());
  renderer->DrawTexturedRect(
      &g_pc_indicator_texture,
      Rect(left_margin, active_ * line_height,
                 indicator_width, indicator_height),
      0, 0, 1, 1);
      */
}
