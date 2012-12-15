// Copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef SG_STACK_VIEW_H_
#define SG_STACK_VIEW_H_

#include <vector>

#include "base/string16.h"
#include "sg/backend/backend.h"
#include "sg/render/font.h"
#include "sg/ui/contents.h"
#include "sg/ui/tree_view_helper.h"

namespace Gwen { namespace Skin { class Base; }}
class Skin;

class StackView : public Contents, public TreeViewHelperDataProvider {
 public:
  explicit StackView(const Skin& skin);

  virtual void Render(Renderer* renderer);

  virtual void SetData(const std::vector<FrameData>& frames, int active);

  // Implementation of TreeViewHelperDataProvider:
  virtual double GetColumnWidth(int column) OVERRIDE;
  virtual void SetColumnWidth(int column, double width) OVERRIDE;
  virtual string16 GetColumnTitle(int column) OVERRIDE;

  virtual int GetNodeChildCount(const std::string& node) OVERRIDE;
  virtual std::string GetIdForChild(
      const std::string& node, int child) OVERRIDE;
  virtual string16 GetNodeDataForColumn(
      const std::string& node, int column) OVERRIDE;
  virtual NodeExpansionState GetNodeExpandability(
      const std::string& node) OVERRIDE;
  virtual void SetNodeExpansionState(
      const std::string& node, NodeExpansionState state) OVERRIDE;
  virtual Size GetTreeViewScreenSize() OVERRIDE;

 private:
  std::vector<std::vector<string16> > lines_;
  int active_;

  TreeViewHelper tree_view_;
  double column_widths_[3];
  Size tree_view_screen_size_;
};

#endif  // SG_STACK_VIEW_H_
