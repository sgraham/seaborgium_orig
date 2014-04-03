// Copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef SG_LOCALS_VIEW_H_
#define SG_LOCALS_VIEW_H_

#include <map>
#include <string>
#include <vector>

#include "sg/backend/backend.h"
#include "sg/base/string16.h"
#include "sg/debug_presenter_display.h"
#include "sg/render/font.h"
#include "sg/ui/dockable.h"
#include "sg/ui/tree_view_helper.h"

class DebugPresenterNotify;

class LocalsView : public Dockable, public TreeViewHelperDataProvider {
 public:
  LocalsView();

  virtual void Render(Renderer* renderer);

  void AddChild(const std::string& parent_id, const std::string& child_id);
  void SetNodeData(const std::string& id,
                   const string16* expression,
                   const string16* value,
                   const string16* type,
                   const bool* has_children);
  void RemoveNode(const std::string& id);
  // See also some of TreeViewHelperDataProvider below.

  void SetDebugPresenterNotify(DebugPresenterNotify* notify);

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

  // Implementation of InputHandler:
  virtual bool NotifyKey(
      InputKey key, bool down, const InputModifiers& modifiers) OVERRIDE;
  virtual bool NotifyMouseMoved(
      int x, int y, int dx, int dy, const InputModifiers& modifiers) OVERRIDE;
  virtual bool NotifyMouseWheel(
      int delta, const InputModifiers& modifiers) OVERRIDE;
  virtual bool NotifyMouseButton(
      int index, bool down, const InputModifiers& modifiers) OVERRIDE;
  virtual bool WantKeyEvents() OVERRIDE { return true; }
  virtual bool WantMouseEvents() OVERRIDE { return true; }

 private:
  // Map from id to list of children.
  std::map<std::string, std::vector<std::string> > children_;

  struct VariableData {
    string16 expression;
    string16 value;
    string16 type;
    NodeExpansionState expansion_state;
    std::string parent_id;
  };
  // Data for each node, mapped by id.
  std::map<std::string, VariableData> node_data_;

  DebugPresenterNotify* notify_;

  TreeViewHelper tree_view_;
  double column_widths_[3];
};

#endif  // SG_LOCALS_VIEW_H_
