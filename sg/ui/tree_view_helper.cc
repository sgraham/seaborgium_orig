// Copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "sg/ui/tree_view_helper.h"

#include "base/logging.h"
#include "base/memory/scoped_ptr.h"
#include "sg/render/renderer.h"
#include "sg/render/scoped_render_offset.h"
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
  buttons_width_ = num_pixels_in_row_;
}

TreeViewHelper::~TreeViewHelper() {
}

int TreeViewHelper::GetYOffsetToFirstRow() const {
  return num_pixels_in_row_ + kHeaderPadding * 2;
}

int TreeViewHelper::GetStartXForColumn(int column) {
  if (column == 0) {
    if (requires_buttons_)
      return buttons_width_;
    return 0;
  } else {
    int width = data_provider_->GetTreeViewScreenSize().w;
    if (requires_buttons_)
      width -= buttons_width_;
    return data_provider_->GetColumnWidth(column - 1) * width;
  }
}

void TreeViewHelper::RenderTree(Renderer* renderer, const Skin& skin) {
  Size screen_size = data_provider_->GetTreeViewScreenSize();
  int height_of_header = GetYOffsetToFirstRow();

  // Header.
  renderer->SetDrawColor(skin.GetColorScheme().margin());
  renderer->DrawFilledRect(Rect(0, 0, screen_size.w, height_of_header));
  renderer->SetDrawColor(skin.GetColorScheme().border());
  renderer->DrawHorizontalLine(0, 0, screen_size.w);
  renderer->DrawFilledRect(Rect(0, height_of_header - 1, screen_size.w, 1));
  for (int i = 1; i < num_columns_; ++i)
    renderer->DrawVerticalLine(GetStartXForColumn(i), 0, screen_size.h);

  // Titles.
  renderer->SetDrawColor(skin.GetColorScheme().margin_text());
  for (int i = 0; i < num_columns_; ++i) {
    int x = GetStartXForColumn(i) + kFromSidePadding;
    const string16& title = data_provider_->GetColumnTitle(i);
    renderer->RenderText(skin.ui_font(), Point(x, kHeaderPadding), title);
  }

  requires_buttons_ = RequiresExpansionButtons();
  last_rendered_buttons_.clear();

  // And contents.
  ScopedRenderOffset offset_title(renderer, 0, height_of_header);
  renderer->SetDrawColor(skin.GetColorScheme().text());
  int y = 0;
  RenderNodes(renderer, skin, std::string(), &y, 0);
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
    NodeExpansionState expansion_state =
        data_provider_->GetNodeExpandability(child_id);
    if (expansion_state == kCollapsed ||
        expansion_state == kExpanded) {
      const Texture* texture = expansion_state == kCollapsed ?
          skin.tree_collapsed_texture() : skin.tree_expanded_texture();
      Rect button_rect = Rect(
          (buttons_width_ - texture->width) / 2,
          (buttons_width_ - texture->height) / 2 + *y,
          texture->width, texture->height);
      renderer->DrawTexturedRect(texture, button_rect, 0, 0, 1, 1);
      renderer->TranslateByRenderOffset(&button_rect);
      last_rendered_buttons_.push_back(RectAndId(button_rect, child_id));
    }
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

bool TreeViewHelper::RequiresExpansionButtons() {
  std::string root;
  int count = data_provider_->GetNodeChildCount(root);
  for (int i = 0; i < count; ++i) {
    std::string child_id = data_provider_->GetIdForChild(root, i);
    if (data_provider_->GetNodeExpandability(child_id) !=
        kNotExpandable)
      return true;
  }
  return false;
}

bool TreeViewHelper::NotifyKey(
    InputKey key, bool down, const InputModifiers& modifiers) {
  return false;
}

bool TreeViewHelper::NotifyMouseMoved(
    int x, int y, int dx, int dy, const InputModifiers& modifiers) {
  mouse_position_ = Point(x, y);
  return false;
}

bool TreeViewHelper::NotifyMouseWheel(
    int delta, const InputModifiers& modifiers) {
  return false;
}

bool TreeViewHelper::NotifyMouseButton(
    int index, bool down, const InputModifiers& modifiers) {
  bool modified = false;
  for (size_t i = 0; i < last_rendered_buttons_.size(); ++i) {
    if (last_rendered_buttons_[i].rect.Contains(mouse_position_)) {
      NodeExpansionState expansion_state =
          data_provider_->GetNodeExpandability(last_rendered_buttons_[i].id);
      CHECK(expansion_state == kCollapsed || expansion_state == kExpanded);
      expansion_state = expansion_state == kCollapsed ? kExpanded : kCollapsed;
      data_provider_->SetNodeExpansionState(
          last_rendered_buttons_[i].id, expansion_state);
      modified = true;
    }
  }
  return modified;
}
