// Copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "sg/ui/docking_split_container.h"

#include "base/logging.h"

DockingSplitContainer::DockingSplitContainer(
    DockingSplitDirection direction, Dockable* left, Dockable* right)
    : direction_(direction),
      fraction_(0.5),
      left_(left),
      right_(right) {
}

DockingSplitContainer::~DockingSplitContainer() {
}

void DockingSplitContainer::SplitChild(
    DockingSplitDirection direction, Dockable* left, Dockable* right) {
  scoped_ptr<Dockable>* to_replace;
  if (left_.get() == left || left_.get() == right) {
    to_replace = &left_;
  } else {
    CHECK(right_.get() == left || right_.get() == right);
    to_replace = &right_;
  }
  to_replace->release(); // We're going re-own this pointer on the next line.
  DockingSplitContainer* replacement =
    new DockingSplitContainer(direction, left, right);
  to_replace->reset(replacement);
  left->set_parent(replacement);
  right->set_parent(replacement);
}
