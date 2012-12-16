// Copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "sg/ui/docking_workspace.h"

#include "sg/ui/docking_split_container.h"

void DockingWorkspace::SetRoot(Dockable* root) {
  DockingSplitContainer* dummy =
      new DockingSplitContainer(kUnknown, root, NULL);
  root_.reset(dummy);
  root->set_parent(dummy);
}

Dockable* DockingWorkspace::GetRoot() {
  return root_->left();
}
