// Copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef SG_LOCALS_VIEW_H_
#define SG_LOCALS_VIEW_H_

#include <string>
#include <vector>

#include "base/string16.h"
#include "sg/backend/backend.h"
#include "sg/debug_presenter_display.h"
#include "sg/render/font.h"
#include "sg/ui/dockable.h"
#include "sg/ui/tree_view_helper.h"

class LocalsView : public Dockable, public TreeViewHelperDataProvider {
 public:
  LocalsView();

  virtual void Render(Renderer* renderer);

  int NumLocals();
  DebugPresenterVariable GetLocal(int local);
  void SetLocal(int local, const DebugPresenterVariable& variable);
  void RemoveLocal(int local);
  void SetLocalValue(
      const std::string& id, bool has_children, const string16& value);

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
  struct VariableData {
    DebugPresenterVariable debug_presenter_variable;
    string16 value;
    NodeExpansionState expansion_state;
  };
  //VariableData FindExistingOrCreateVariableData(const TypeNameValue& local);
  //bool IsTypeExpandable(const string16& type);

  std::vector<VariableData> lines_;

  TreeViewHelper tree_view_;
  double column_widths_[3];
};

#endif  // SG_LOCALS_VIEW_H_
