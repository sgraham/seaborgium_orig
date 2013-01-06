// Copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "sg/locals_view.h"

#include "base/logging.h"
#include "base/string_number_conversions.h"
#include "base/string_util.h"
#include "sg/display_util.h"
#include "sg/debug_presenter_notify.h"
#include "sg/render/renderer.h"
#include "sg/ui/skin.h"

LocalsView::LocalsView()
    : tree_view_(this, Skin::current().text_line_height(), 3),
      notify_(NULL) {
  // TODO(config): Save this.
  column_widths_[0] = .25;
  column_widths_[1] = .80;
  column_widths_[2] = 1.;
}

void LocalsView::AddChild(
    const std::string& parent_id, const std::string& child_id) {
}

void LocalsView::SetNodeData(
    const std::string& id,
    const string16* expression,
    const string16* value,
    const string16* type) {
}

void LocalsView::RemoveNode(const std::string& id) {
}

  /*
int LocalsView::NumLocals() {
  return static_cast<int>(lines_.size());
}

DebugPresenterVariable LocalsView::GetLocal(int local) {
  DCHECK(local >= 0 && local < lines_.size());
  return lines_[local].debug_presenter_variable;
}

void LocalsView::SetLocal(int local, const DebugPresenterVariable& variable) {
  DCHECK(local >= 0);
  DCHECK(local <= lines_.size()); // == is OK and means append.
  if (local == lines_.size()) {
    VariableData variable_data;
    variable_data.debug_presenter_variable = variable;
    variable_data.expansion_state = kNotExpandable;
    lines_.push_back(variable_data);
  } else {
    lines_[local].debug_presenter_variable = variable;
  }
}

void LocalsView::RemoveLocal(int local) {
  lines_.erase(lines_.begin() + local);
}

void LocalsView::SetLocalValue(const std::string& id, const string16& value) {
  for (size_t i = 0; i < lines_.size(); ++i) {
    if (lines_[i].debug_presenter_variable.backend_id() == id) {
      lines_[i].value = value;
      break;
    }
  }
}

void LocalsView::SetLocalHasChildren(
    const std::string& id, bool has_children) {
  for (size_t i = 0; i < lines_.size(); ++i) {
    if (lines_[i].debug_presenter_variable.backend_id() == id) {
      if (has_children && lines_[i].expansion_state == kNotExpandable)
        lines_[i].expansion_state = kCollapsed;
      else if (!has_children)
        lines_[i].expansion_state = kNotExpandable;
      break;
    }
  }
}
*/

void LocalsView::SetDebugPresenterNotify(DebugPresenterNotify* notify) {
  notify_ = notify;
}

void LocalsView::Render(Renderer* renderer) {
  const Skin& skin = Skin::current();

  renderer->SetDrawColor(skin.GetColorScheme().background());
  renderer->DrawFilledRect(Rect(0, 0, Width(), Height()));

  // TODO(rendering): Scroll helper.
  tree_view_.RenderTree(renderer, skin);
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
  return children_[node].size();
}

std::string LocalsView::GetIdForChild(const std::string& node, int child) {
  return base::IntToString(child);
}

string16 LocalsView::GetNodeDataForColumn(const std::string& node, int column) {
  if (column == 0)
    return node_data_[node].expression;
  else if (column == 1)
    return node_data_[node].value;
  else if (column == 2)
    return TidyTypeName(node_data_[node].type);
  NOTREACHED();
  return L"";
}

NodeExpansionState LocalsView::GetNodeExpandability(const std::string& node) {
  return node_data_[node].expansion_state;
}

void LocalsView::SetNodeExpansionState(
    const std::string& node, NodeExpansionState state) {
  node_data_[node].expansion_state = state;
  notify_->NotifyVariableExpansionStateChanged(node, state == kExpanded);
}

Size LocalsView::GetTreeViewScreenSize() {
  return Size(GetScreenRect().w, GetScreenRect().h);
}

// TODO(scottmg): For all of these, if the tree view doesn't handle, forward
// on to a scroll helper.
bool LocalsView::NotifyKey(
    InputKey key, bool down, const InputModifiers& modifiers) {
  return tree_view_.NotifyKey(key, down, modifiers);
}

bool LocalsView::NotifyMouseMoved(
    int x, int y, int dx, int dy, const InputModifiers& modifiers) {
  return tree_view_.NotifyMouseMoved(x, y, dx, dy, modifiers);
}

bool LocalsView::NotifyMouseWheel(
    int delta, const InputModifiers& modifiers) {
  return tree_view_.NotifyMouseWheel(delta, modifiers);
}

bool LocalsView::NotifyMouseButton(
    int index, bool down, const InputModifiers& modifiers) {
  // TODO(scottmg): This is a sketcho bool. Might need to rethink the "helper".
  bool modified = tree_view_.NotifyMouseButton(index, down, modifiers);
  if (modified)
    Invalidate();
  return modified;
}
