// Copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "sg/ui/docking_split_container.h"

#include "base/logging.h"
#include "sg/render/renderer.h"
#include "sg/ui/skin.h"

namespace {

int gSplitterWidth = 4;

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
  replacement->SetScreenRect(GetScreenRect());
}

void DockingSplitContainer::SetScreenRect(const Rect& rect) {
  Dockable::SetScreenRect(rect);
  if (direction_ == kSplitVertical) {
    int width = GetScreenRect().w - gSplitterWidth;
    int width_for_left = width * fraction_;
    int width_for_right = width - width_for_left;
    left_->SetScreenRect(Rect(rect.x, rect.y, width_for_left, rect.h));
    right_->SetScreenRect(
        Rect(rect.x + width_for_left + gSplitterWidth, rect.y,
             width_for_right, rect.h));
  } else if (direction_ == kSplitHorizontal) {
    int height = GetScreenRect().h - gSplitterWidth;
    int height_for_left = height * fraction_;
    int height_for_right = height - height_for_left;
    left_->SetScreenRect(Rect(rect.x, rect.y, rect.w, height_for_left));
    right_->SetScreenRect(
        Rect(rect.x, rect.y + height_for_left + gSplitterWidth,
             rect.w, height_for_right));
  } else {
    CHECK(direction_ == kSplitNoneRoot);
    if (left_.get())
      left_->SetScreenRect(GetScreenRect());
  }
}

Rect DockingSplitContainer::GetRectForSplitter() {
  if (direction_ == kSplitVertical) {
    int width = GetScreenRect().w - gSplitterWidth;
    int width_for_left = width * fraction_;
    return Rect(width_for_left, 0, gSplitterWidth, GetScreenRect().h);
  } else if (direction_ == kSplitHorizontal) {
    int height = GetScreenRect().h - gSplitterWidth;
    int height_for_top = height * fraction_;
    return Rect(0, height_for_top, GetScreenRect().w, gSplitterWidth);
  }
  return Rect();
}

void DockingSplitContainer::Render(Renderer* renderer, const Skin& skin) {
  Point old_offset = renderer->GetRenderOffset();

  renderer->SetRenderOffset(Point(left_->X(), left_->Y()));
  left_->Render(renderer, skin);
  renderer->SetRenderOffset(old_offset);

  if (right_.get()) {
    renderer->SetRenderOffset(Point(right_->X(), right_->Y()));
    right_->Render(renderer, skin);
    renderer->SetRenderOffset(old_offset);
  } else {
    CHECK(direction_ == kSplitNoneRoot);
  }

  if (direction_ == kSplitVertical || direction_ == kSplitHorizontal) {
    renderer->SetDrawColor(skin.GetColorScheme().border());
    renderer->DrawFilledRect(GetRectForSplitter());
  }
}

bool DockingSplitContainer::CouldStartDrag(
    const Point& screen_position,
    DragDirection* direction,
    DockingSplitContainer** target) {
  Point client_position = ToClient(screen_position);
  if (GetRectForSplitter().Contains(client_position)) {
    if (target)
      *target = this;
    if (direction_ == kSplitVertical)
      *direction = kDragDirectionLeftRight;
    else if (direction_ == kSplitHorizontal)
      *direction = kDragDirectionUpDown;
    return true;
  } else {
    if (left_->GetScreenRect().Contains(screen_position))
      return left_->CouldStartDrag(screen_position, direction, target);
    if (right_.get()) {
      if (right_->GetScreenRect().Contains(screen_position))
        return right_->CouldStartDrag(screen_position, direction, target);
    }
  }
  return false;
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
