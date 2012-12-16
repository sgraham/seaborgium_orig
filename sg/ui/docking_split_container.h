// Copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef SG_UI_DOCKING_SPLIT_CONTAINER_H_
#define SG_UI_DOCKING_SPLIT_CONTAINER_H_

#include "base/memory/scoped_ptr.h"
#include "sg/ui/dockable.h"

enum DockingSplitDirection {
  kSplitNoneRoot,
  kSplitVertical,
  kSplitHorizontal,
};

class DockingSplitContainer : public Dockable {
 public:
  DockingSplitContainer(DockingSplitDirection direction,
                        Dockable* left, Dockable* right);
  virtual ~DockingSplitContainer();

  static void SetSplitterWidth(int width);
  static int GetSplitterWidth();

  virtual bool IsContainer() const { return true; }

  // Finds left or right in its children, and replaces that child with a new
  // container split in |direction| containing |left| and |right| in that
  // order.
  void SplitChild(DockingSplitDirection direction,
                  Dockable* left,
                  Dockable* right);

  void SetScreenRect(const Rect& rect) OVERRIDE;

  DockingSplitDirection direction() const { return direction_; }
  double fraction() const { return fraction_; }
  Dockable* left() { return left_.get(); }
  Dockable* right() { return right_.get(); }

  // Hokey method only used for DockingWorkspace that uses left as root.
  void ReplaceLeft(Dockable* left);

 private:
  DockingSplitDirection direction_;
  double fraction_;

  // Named left/right for simplicity, but "left" is also "top" if the split is
  // horizontal rather than vertical.
  scoped_ptr<Dockable> left_;
  scoped_ptr<Dockable> right_;
};

#endif  // SG_UI_DOCKING_SPLIT_CONTAINER_H_
