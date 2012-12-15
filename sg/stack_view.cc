// Copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "sg/stack_view.h"

#include <algorithm>

#include "base/logging.h"
#include "base/string_number_conversions.h"
#include "base/utf_string_conversions.h"
#include "base/string_util.h"
#include "re2/re2.h"
#include "sg/render/texture.h"
#include "sg/ui/skin.h"

namespace {

// TODO(config):
// TODO(rendering): Font line height.
const int g_line_height = 17;
Texture g_pc_indicator_texture;

string16 TidyTypeName(const string16& type) {
  // Change "char *" to "char*"
  // TODO: References.
  if (type.find(L"*") == string16::npos)  // Early out to avoid unnecessary RE.
    return type;
  std::string utf8 = UTF16ToUTF8(type);
  if (RE2::Replace(&utf8, "(.*?)\\s+(\\*+)", "\\1\\2"))
    return UTF8ToUTF16(utf8);
  return type;
}

}  // namespace

StackView::StackView(const Skin& skin) : Contents(skin), active_(-1) {
  // TODO(rendering): Share with source view.
  font_.facename = L"Consolas";
  font_.size = 13.f;
  // TODO(config): Share with source view.
  g_pc_indicator_texture.name = L"art/pc-location.png";
}

void StackView::SetData(const std::vector<FrameData>& frames, int active) {
  lines_.clear();
  for (size_t i = 0; i < frames.size(); ++i) {
    const FrameData& frame = frames[i];
    wchar_t buf[64];
    base::swprintf(buf, sizeof(buf), L"0x%lx", frame.address);
    string16 arguments = L"(";
    for (size_t j = 0; j < frame.arguments.size(); ++j) {
      const TypeNameValue& argument = frame.arguments[j];
      arguments += TidyTypeName(argument.type) + L" " +
                   argument.name;
      // Not currently including "= argument.value".
      if (j != frame.arguments.size() - 1)
        arguments += L", ";
    }
    arguments += L")";
    lines_.push_back(frame.function + arguments +
                     L", " + frame.filename +
                     L":" + base::IntToString16(frame.line_number) +
                     L" @ " + string16(buf));
  }
  active_ = 0;
  Invalidate();
}

void StackView::Render(Renderer* renderer) {
  const Skin& skin = Contents::GetSkin();

  // TODO(rendering): Hacky.
  if (!g_pc_indicator_texture.data) {
    renderer->LoadTexture(&g_pc_indicator_texture);
  }

  renderer->SetDrawColor(skin.GetColorScheme().background());
  renderer->DrawFilledRect(Rect(0, 0, Width(), Height()));

  static const int left_margin = 5;
  static const int right_margin = 5;
  static const int indicator_width = g_line_height;
  static const int indicator_height = g_line_height;
  static const int full_margin_width =
      left_margin + indicator_width + right_margin;

  renderer->SetDrawColor(skin.GetColorScheme().margin());
  renderer->DrawFilledRect(Rect(0, 0, full_margin_width, Height()));

  // TODO(rendering): Generic scroll pane.
  renderer->SetDrawColor(skin.GetColorScheme().text());
  for (size_t i = 0; i < lines_.size(); ++i) {
    renderer->RenderText(
        &font_, Point(full_margin_width, i * g_line_height), lines_[i]);
  }

  renderer->SetDrawColor(skin.GetColorScheme().pc_indicator());
  renderer->DrawTexturedRect(
      &g_pc_indicator_texture,
      Rect(left_margin, active_ * g_line_height,
                 indicator_width, indicator_height),
      0, 0, 1, 1);
}
