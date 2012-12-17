// Copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "sg/ui/docking_workspace.h"

#include "sg/ui/docking_split_container.h"

DockingWorkspace::DockingWorkspace() {
  root_.reset(new DockingSplitContainer(kSplitNoneRoot, NULL, NULL));
}

DockingWorkspace::~DockingWorkspace() {
}

void DockingWorkspace::Render(Renderer* renderer) {
  if (root_->left())
    root_->left()->Render(renderer);
}

void DockingWorkspace::SetRoot(Dockable* root) {
  root_->ReplaceLeft(root);
  root->set_parent(root_.get());
}

Dockable* DockingWorkspace::GetRoot() {
  return root_->left();
}

void DockingWorkspace::SetScreenRect(const Rect& rect) {
  root_->SetScreenRect(rect);
}

Rect DockingWorkspace::GetScreenRect() {
  return root_->GetScreenRect();
}

bool DockingWorkspace::CouldStartDrag(DragSetup* drag_setup) {
  return root_->CouldStartDrag(drag_setup);
}

