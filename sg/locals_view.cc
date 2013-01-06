// Copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "sg/locals_view.h"

#include "base/logging.h"
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
  children_[parent_id].push_back(child_id);
  VariableData data;
  data.expansion_state = kNotExpandable;
  data.parent_id = parent_id;
  node_data_[child_id] = data;
}

void LocalsView::SetNodeData(
    const std::string& id,
    const string16* expression,
    const string16* value,
    const string16* type,
    const bool* has_children) {
  DCHECK(node_data_.find(id) != node_data_.end());
  std::map<std::string, VariableData>::iterator i = node_data_.find(id);
  VariableData* variable_data = &i->second;
  if (expression)
    variable_data->expression = *expression;
  if (value)
    variable_data->value = *value;
  if (type)
    variable_data->type = *type;
  if (has_children) {
    variable_data->expansion_state =
        *has_children ? kCollapsed : kNotExpandable;
  }
}

void LocalsView::RemoveNode(const std::string& id) {
  for (size_t i = 0; i < children_[id].size(); ++i)
    RemoveNode(children_[id][i]);
  children_.erase(node_data_[id].parent_id);
  children_.erase(id);
  node_data_.erase(id);
}

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
  return children_[node][child];
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
