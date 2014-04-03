// Copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef SG_UI_TREE_VIEW_HELPER_H_
#define SG_UI_TREE_VIEW_HELPER_H_

#include <string>
#include <vector>

#include "sg/base/string16.h"
#include "sg/basic_geometric_types.h"
#include "sg/ui/input.h"
class Font;
class Renderer;
class Skin;

enum NodeExpansionState {
  kNotExpandable,
  kCollapsed,
  kExpanded,
};

class TreeViewHelperDataProvider {
 public:
  virtual ~TreeViewHelperDataProvider() {}

  virtual double GetColumnWidth(int column) = 0;
  virtual void SetColumnWidth(int column, double width) = 0;
  virtual string16 GetColumnTitle(int column) = 0;

  // |node| can be L"" to indicate top level.
  virtual int GetNodeChildCount(const std::string& node) = 0;
  virtual std::string GetIdForChild(const std::string& node, int child) = 0;
  virtual string16 GetNodeDataForColumn(
      const std::string& node, int column) = 0;
  virtual NodeExpansionState GetNodeExpandability(const std::string& node) = 0;
  virtual void SetNodeExpansionState(
      const std::string& node, NodeExpansionState state) = 0;

  virtual Size GetTreeViewScreenSize() = 0;
};

// Draws a tree grid view and handles column resizing and tree
// expand/collapsing.
class TreeViewHelper {
 public:
  TreeViewHelper(TreeViewHelperDataProvider* data_provider,
                 int num_pixels_in_row,
                 int num_columns);
  virtual ~TreeViewHelper();

  void RenderTree(Renderer* renderer, const Skin& skin);

  int GetRowHeight() const { return num_pixels_in_row_; }
  int GetYOffsetToFirstRow() const;

  bool NotifyKey(InputKey key, bool down, const InputModifiers& modifiers);
  bool NotifyMouseMoved(
      int x, int y, int dx, int dy, const InputModifiers& modifiers);
  bool NotifyMouseWheel(int delta, const InputModifiers& modifiers);
  bool NotifyMouseButton(int index, bool down, const InputModifiers& modifiers);

 private:
  struct RectAndId {
    RectAndId(const Rect& rect, const std::string& id) : rect(rect), id(id) {
    }
    Rect rect;
    std::string id;
  };

  int GetStartXForColumn(int column);
  void RenderNodes(Renderer* renderer,
                   const Skin& skin,
                   const std::string& root,
                   int* y,
                   int indent);
  bool RequiresExpansionButtons();

  TreeViewHelperDataProvider* data_provider_;
  int num_pixels_in_row_;
  int num_columns_;
  int indent_size_;
  int buttons_width_;
  bool requires_buttons_;
  std::vector<RectAndId> last_rendered_buttons_;
  Point mouse_position_;
};

#endif  // SG_UI_TREE_VIEW_HELPER_H_
