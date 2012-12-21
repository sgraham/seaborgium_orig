// Copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "sg/locals_view.h"

#include "base/logging.h"
#include "base/string_number_conversions.h"
#include "base/string_util.h"
#include "sg/display_util.h"
#include "sg/render/renderer.h"
#include "sg/ui/skin.h"

LocalsView::LocalsView()
    : tree_view_(this, Skin::current().text_line_height(), 3) {
  // TODO(config): Save this.
  column_widths_[0] = .25;
  column_widths_[1] = .80;
  column_widths_[2] = 1.;
}

void LocalsView::SetData(const std::vector<TypeNameValue>& locals) {
  std::vector<VariableData> new_lines;
  for (size_t i = 0; i < locals.size(); ++i) {
    VariableData variable_data = FindExistingOrCreateVariableData(locals[i]);
    variable_data.value = locals[i].value;
    new_lines.push_back(variable_data);
  }
  lines_ = new_lines;
  Invalidate();
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
    return TidyTypeName(lines_[node_index].type);
  NOTREACHED();
  return L"";
}

NodeExpansionState LocalsView::GetNodeExpandability(const std::string& node) {
  int node_index;
  CHECK(base::StringToInt(node, &node_index));
  return lines_[node_index].expansion_state;
}

void LocalsView::SetNodeExpansionState(
    const std::string& node, NodeExpansionState state) {
}

Size LocalsView::GetTreeViewScreenSize() {
  return Size(GetScreenRect().w, GetScreenRect().h);
}

LocalsView::VariableData LocalsView::FindExistingOrCreateVariableData(
    const TypeNameValue& local) {
  for (size_t i = 0; i < lines_.size(); ++i) {
    if (lines_[i].name == local.name && lines_[i].type == local.type)
      return lines_[i];
  }
  VariableData variable_data(local);
  if (IsTypeExpandable(variable_data.type))
    variable_data.expansion_state = kCollapsed;
  return variable_data;
}

bool LocalsView::IsTypeExpandable(const string16& type) {
  // TODO(scottmg): Hmm. Need to lift this into a Type for all these
  // manipulations. Probably requires backend queries.
  if (EndsWith(type, L"*", true))
    return true;
  if (type == L"int" || type == L"float" || type == L"double" || type == L"char") {
    return false;
  }
  return true;
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
  return tree_view_.NotifyMouseButton(index, down, modifiers);
}

LocalsView::VariableData::VariableData(const TypeNameValue& type_name_value)
    : type(type_name_value.type),
      name(type_name_value.name),
      value(type_name_value.value),
      expansion_state(kNotExpandable) {
}
