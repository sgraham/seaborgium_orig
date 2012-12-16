// Copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef SG_UI_DOCKING_SPLIT_CONTAINER_H_
#define SG_UI_DOCKING_SPLIT_CONTAINER_H_

#include "base/memory/scoped_ptr.h"
#include "sg/ui/dockable.h"

enum DockingSplitDirection {
  kUnknown,
  kVertical,
  kHorizontal,
};

class DockingSplitContainer : public Dockable {
 public:
  DockingSplitContainer(DockingSplitDirection direction,
                        Dockable* left, Dockable* right);
  virtual ~DockingSplitContainer();

  virtual bool IsContainer() const { return true; }

  // Finds left or right in its children, and replaces that child with a new
  // container split in |direction| containing |left| and |right| in that
  // order.
  void SplitChild(DockingSplitDirection direction,
                  Dockable* left,
                  Dockable* right);

  DockingSplitDirection direction() const { return direction_; }
  double fraction() const { return fraction_; }
  Dockable* left() { return left_.get(); }
  Dockable* right() { return right_.get(); }

 private:
  DockingSplitDirection direction_;
  double fraction_;

  // Named left/right for simplicity, but "left" is also "top" if the split is
  // horizontal rather than vertical.
  scoped_ptr<Dockable> left_;
  scoped_ptr<Dockable> right_;
};

#endif  // SG_UI_DOCKING_SPLIT_CONTAINER_H_
