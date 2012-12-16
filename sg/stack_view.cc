// Copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "sg/stack_view.h"

#include <algorithm>

#include "base/logging.h"
#include "base/string_number_conversions.h"
#include "base/string_util.h"
#include "sg/display_util.h"
#include "sg/render/renderer.h"
#include "sg/render/texture.h"
#include "sg/ui/skin.h"

namespace {

// TODO(config):
// TODO(rendering): Font line height.
const int g_line_height = 17;
Texture g_pc_indicator_texture;

}  // namespace

StackView::StackView()
    : active_(-1),
      tree_view_(this, g_line_height, arraysize(column_widths_)) {
  // TODO(config): Share with source view.
  g_pc_indicator_texture.name = L"art/pc-location.png";
  // TODO(config): Save this.
  column_widths_[0] = .5;
  column_widths_[1] = .80;
  column_widths_[2] = 1.;
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
    std::vector<string16> columns;
    columns.push_back(frame.function + arguments);
    columns.push_back(ToPlatformFileAndLine(frame.filename, frame.line_number));
    columns.push_back(string16(buf));
    lines_.push_back(columns);
  }
  active_ = 0;
  Invalidate();
}

void StackView::Render(Renderer* renderer, const Skin& skin) {
  // TODO(rendering): Hacky.
  if (!g_pc_indicator_texture.data) {
    renderer->LoadTexture(&g_pc_indicator_texture);
  }

  renderer->SetDrawColor(skin.GetColorScheme().background());
  renderer->DrawFilledRect(Rect(0, 0, Width(), Height()));

  static const int left_margin = 5;
  static const int right_margin = 5;
  static const int indicator_width = tree_view_.GetRowHeight();
  static const int indicator_height = tree_view_.GetRowHeight();
  static const int full_margin_width =
      left_margin + indicator_width + right_margin;

  renderer->SetDrawColor(skin.GetColorScheme().margin());
  renderer->DrawFilledRect(Rect(0, 0, full_margin_width, Height()));

  renderer->SetDrawColor(skin.GetColorScheme().pc_indicator());
  renderer->DrawTexturedRect(
      &g_pc_indicator_texture,
      Rect(left_margin,
           active_ * g_line_height + tree_view_.GetYOffsetToFirstRow(),
           indicator_width, indicator_height),
      0, 0, 1, 1);

  Point old_render_offset = renderer->GetRenderOffset();
  renderer->AddRenderOffset(Point(full_margin_width, 0));
  tree_view_screen_size_.w = GetScreenRect().w - full_margin_width;
  tree_view_screen_size_.h = GetScreenRect().h;
  tree_view_.RenderTree(renderer, skin);
  renderer->SetRenderOffset(old_render_offset);
}

double StackView::GetColumnWidth(int column) {
  return column_widths_[column];
}

void StackView::SetColumnWidth(int column, double width) {
  column_widths_[column] = width;
}

string16 StackView::GetColumnTitle(int column) {
  switch (column) {
    case 0:
      return L"Function";
    case 1:
      return L"Location";
    case 2:
      return L"Address";
    default:
      NOTREACHED();
      return L"";
  }
}

int StackView::GetNodeChildCount(const std::string& node) {
  if (node == "")
    return static_cast<int>(lines_.size());
  else
    return 0;
}

std::string StackView::GetIdForChild(const std::string& node, int child) {
  return base::IntToString(child);
}

string16 StackView::GetNodeDataForColumn(const std::string& node, int column) {
  int node_index;
  CHECK(base::StringToInt(node, &node_index));
  return lines_[node_index][column];
}

NodeExpansionState StackView::GetNodeExpandability(const std::string& node) {
  return kNotExpandable;
}

void StackView::SetNodeExpansionState(
      const std::string& node, NodeExpansionState state) {
  NOTREACHED();
}

Size StackView::GetTreeViewScreenSize() {
  return tree_view_screen_size_;
}
