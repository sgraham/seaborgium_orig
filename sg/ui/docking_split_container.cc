// Copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "sg/ui/docking_split_container.h"

#include "base/logging.h"

namespace {

int gSplitterWidth = 3;

}  // namespace

void DockingSplitContainer::SetSplitterWidth(int width) {
  gSplitterWidth = width;
}

int DockingSplitContainer::GetSplitterWidth() {
  return gSplitterWidth;
}

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
  replacement->SetScreenRect(Dockable::GetScreenRect());
}

void DockingSplitContainer::SetScreenRect(const Rect& rect) {
  Dockable::SetScreenRect(rect);
  if (direction_ == kSplitVertical) {
    int width = Dockable::GetScreenRect().w - gSplitterWidth;
    int width_for_left = width * fraction_;
    int width_for_right = width - width_for_left;
    left_->SetScreenRect(Rect(rect.x, rect.y, width_for_left, rect.h));
    right_->SetScreenRect(
        Rect(rect.x + width_for_left + gSplitterWidth, rect.y,
             width_for_right, rect.h));
  } else if (direction_ == kSplitHorizontal) {
    int height = Dockable::GetScreenRect().h - gSplitterWidth;
    int height_for_left = height * fraction_;
    int height_for_right = height - height_for_left;
    left_->SetScreenRect(Rect(rect.x, rect.y, rect.w, height_for_left));
    right_->SetScreenRect(
        Rect(rect.x, rect.y + height_for_left + gSplitterWidth,
             rect.w, height_for_right));
  } else {
    CHECK(direction_ == kSplitNoneRoot);
    if (left_.get())
      left_->SetScreenRect(Dockable::GetScreenRect());
  }
}

void DockingSplitContainer::SetFraction(double fraction) {
  fraction_ = fraction;
  // Propagate fraction changes to children.
  SetScreenRect(GetScreenRect());
}

void DockingSplitContainer::ReplaceLeft(Dockable* left) {
  CHECK(direction_ == kSplitNoneRoot && !right_.get());
  left_.reset(left);
  left->SetScreenRect(Dockable::GetScreenRect());
}
