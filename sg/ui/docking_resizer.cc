// Copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "sg/ui/docking_resizer.h"

#include "base/logging.h"
#include "sg/ui/docking_split_container.h"

DockingResizer::DockingResizer(DockingSplitContainer* resizing)
    : resizing_(resizing) {
  // Only one of these is relevant, so we handle both vertical and horizontal
  // by making the point on the splitter be the same fraction.
  int parent_size = ParentSize();
  initial_location_ = Point(
      parent_size * resizing_->fraction(),
      parent_size * resizing_->fraction());
}

DockingResizer::~DockingResizer() {
}

void DockingResizer::Drag(const Point& point) {
  int parent_size = ParentSize();
  int clamped = std::min(std::max(ComponentForDirection(point), 0),
                         parent_size);
  resizing_->SetFraction(static_cast<double>(clamped) / parent_size);
}

void DockingResizer::CancelDrag() {
  int parent_size = ParentSize();
  resizing_->SetFraction(
      static_cast<double>(initial_location_.x) / parent_size);
}

int DockingResizer::ParentSize() {
  const Rect& parent_rect = resizing_->GetScreenRect();
  int splitter_width = DockingSplitContainer::GetSplitterWidth();
  CHECK(resizing_->direction() == kSplitVertical ||
        resizing_->direction() == kSplitHorizontal);
  if (resizing_->direction() == kSplitVertical)
    return parent_rect.w - splitter_width;
  else
    return parent_rect.h - splitter_width;
}

int DockingResizer::ComponentForDirection(const Point& point) {
  CHECK(resizing_->direction() == kSplitVertical ||
        resizing_->direction() == kSplitHorizontal);
  if (resizing_->direction() == kSplitVertical)
    return point.x;
  else
    return point.y;
}
