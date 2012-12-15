// Copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef SG_UI_TREE_VIEW_HELPER_H_
#define SG_UI_TREE_VIEW_HELPER_H_

#include <string>

#include "base/string16.h"

enum NodeExpansionState {
  kNotExpandable,
  kCollapsed,
  kExpanded,
};

class TreeViewHelper {
  int GetColumnWidth(int column) = 0;
  void SetColumnWidth(int column, int width) = 0;
  string16 GetColumnTitle(int column) = 0;

  // |node| can be L"" to indicate top level.
  int GetNodeChildCount(const std::string& node) = 0;
  std::string GetIdForChild(const std::string& node, int child) = 0;
  string16 GetNodeDataForColumn(const std::string& node, int column) = 0;
  NodeExpansionState GetNodeExpandability(const std::string& node) = 0;
  void SetNodeExpansionState(
      const std::string& node, NodeExpansionState state) = 0;
};

#endif  // SG_UI_TREE_VIEW_HELPER_H_
