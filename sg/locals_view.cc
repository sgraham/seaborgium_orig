// Copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "sg/locals_view.h"

#include "base/logging.h"
#include "base/string_number_conversions.h"
#include "sg/render/renderer.h"
#include "sg/ui/skin.h"

LocalsView::LocalsView()
    : tree_view_(this, Skin::current().text_line_height(), 3) {
  // TODO(config): Save this.
  column_widths_[0] = .5;
  column_widths_[1] = .80;
  column_widths_[2] = 1.;
}

void LocalsView::SetData(const std::vector<TypeNameValue>& locals) {
  lines_.clear();
  // TODO
  for (size_t i = 0; i < locals.size(); ++i) {
    lines_.push_back(locals[i]);
  }
  Invalidate();
}

void LocalsView::Render(Renderer* renderer) {
  const Skin& skin = Skin::current();
  int line_height = skin.text_line_height();

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

  // TODO(rendering): Scroll helper.
  Point old_render_offset = renderer->GetRenderOffset();
  renderer->AddRenderOffset(Point(full_margin_width, 0));
  tree_view_screen_size_.w = GetScreenRect().w - full_margin_width;
  tree_view_screen_size_.h = GetScreenRect().h;
  tree_view_.RenderTree(renderer, skin);
  renderer->SetRenderOffset(old_render_offset);
}

double LocalsView::GetColumnWidth(int column) {
  return column_widths_[column];
}

void LocalsView::SetColumnWidth(int column, double width) {
  column_widths_[column] = width;
}

string16 LocalsView::GetColumnTitle(int column) {
  switch (column) {
    case 0:
      return L"Name";
    case 1:
      return L"Value";
    case 2:
      return L"Type";
    default:
      NOTREACHED();
      return L"";
  }
}

int LocalsView::GetNodeChildCount(const std::string& node) {
  // TODO
  if (node == "")
    return lines_.size();
  return 0;
}

std::string LocalsView::GetIdForChild(const std::string& node, int child) {
  return base::IntToString(child);
}

string16 LocalsView::GetNodeDataForColumn(const std::string& node, int column) {
  int node_index;
  CHECK(base::StringToInt(node, &node_index));
  if (column == 0)
    return lines_[node_index].name;
  else if (column == 1)
    return lines_[node_index].value;
  else if (column == 2)
    return lines_[node_index].type;
  NOTREACHED();
  return L"";
}

NodeExpansionState LocalsView::GetNodeExpandability(const std::string& node) {
  return kNotExpandable;
}

void LocalsView::SetNodeExpansionState(
    const std::string& node, NodeExpansionState state) {
}

Size LocalsView::GetTreeViewScreenSize() {
  return tree_view_screen_size_;
}
