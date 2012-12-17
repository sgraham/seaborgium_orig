// Copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef SG_UI_DOCKING_WORKSPACE_H_
#define SG_UI_DOCKING_WORKSPACE_H_

#include "base/memory/scoped_ptr.h"
#include "sg/ui/dockable.h"

class DockingSplitContainer;

// Top level container holding a tree of |Dockable|s.
class DockingWorkspace {
 public:
  DockingWorkspace();
  virtual ~DockingWorkspace();

  void Render(Renderer* renderer, const Skin& skin);
  void UpdateCursor(const Point& screen_position);

  // Takes ownership.
  void SetRoot(Dockable* root);
  Dockable* GetRoot();

  void SetScreenRect(const Rect& rect);

 private:
  scoped_ptr<DockingSplitContainer> root_;
};

#endif  // SG_UI_DOCKING_WORKSPACE_H_
