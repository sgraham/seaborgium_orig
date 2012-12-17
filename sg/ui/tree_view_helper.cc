// Copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "sg/ui/tree_view_helper.h"

#include "sg/render/renderer.h"
#include "sg/ui/skin.h"

namespace {

const int kHeaderPadding = 3;
const int kFromSidePadding = 3;

}  // namespace

TreeViewHelper::TreeViewHelper(
    TreeViewHelperDataProvider* data_provider,
    int num_pixels_in_row,
    int num_columns)
    : data_provider_(data_provider),
      num_pixels_in_row_(num_pixels_in_row),
      num_columns_(num_columns) {
  indent_size_ = num_pixels_in_row_;
}

TreeViewHelper::~TreeViewHelper() {
}

int TreeViewHelper::GetYOffsetToFirstRow() const {
  return num_pixels_in_row_ + kHeaderPadding * 2;
}

int TreeViewHelper::GetStartXForColumn(int column) {
  int offset = 0;
  if (column > 0)
    offset = data_provider_->GetColumnWidth(column - 1) *
             data_provider_->GetTreeViewScreenSize().w;
  return offset;
}

void TreeViewHelper::RenderTree(Renderer* renderer, const Skin& skin) {
  Size screen_size = data_provider_->GetTreeViewScreenSize();

  int height_of_header = GetYOffsetToFirstRow();
  renderer->SetDrawColor(skin.GetColorScheme().margin());
  renderer->DrawFilledRect(Rect(0, 0, screen_size.w, height_of_header));
  renderer->SetDrawColor(skin.GetColorScheme().border());
  renderer->DrawFilledRect(Rect(0, 0, screen_size.w, 1));
  renderer->DrawFilledRect(Rect(0, height_of_header - 1, screen_size.w, 1));
  for (int i = 0; i < num_columns_; ++i)
    renderer->DrawFilledRect(Rect(GetStartXForColumn(i), 0, 1, screen_size.h));

  renderer->SetDrawColor(skin.GetColorScheme().margin_text());
  for (int i = 0; i < num_columns_; ++i) {
    int x = GetStartXForColumn(i) + kFromSidePadding;
    const string16& title = data_provider_->GetColumnTitle(i);
    renderer->RenderText(skin.ui_font(), Point(x, kHeaderPadding), title);
  }

  renderer->SetDrawColor(skin.GetColorScheme().text());
  int y = height_of_header;
  RenderNodes(renderer, skin, "", &y, 0);
}

void TreeViewHelper::RenderNodes(
    Renderer* renderer,
    const Skin& skin,
    const std::string& root,
    int* y,
    int indent) {
  int count = data_provider_->GetNodeChildCount(root);
  for (int i = 0; i < count; ++i) {
    std::string child_id = data_provider_->GetIdForChild(root, i);
    for (int j = 0; j < num_columns_; ++j) {
      // TODO(rendering): Perhaps dispatch out to customizers here.
      // TODO(rendering): Clip.
      renderer->RenderText(
          skin.ui_font(),
          Point(GetStartXForColumn(j) + kFromSidePadding, *y),
          data_provider_->GetNodeDataForColumn(child_id, j));
    }
    *y += num_pixels_in_row_;
    if (data_provider_->GetNodeExpandability(child_id) == kExpanded)
      RenderNodes(renderer, skin, child_id, y, indent + indent_size_);
  }
}
