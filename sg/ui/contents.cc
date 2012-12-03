// Copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "sg/ui/contents.h"

Contents::Contents(const Skin& skin) : parent_(NULL), skin_(skin) {
}

Contents::~Contents() {
}

void Contents::SetParent(Contents* parent) {
  parent_ = parent;
}

Contents* Contents::GetParent() const {
  return parent_;
}

void Contents::SetScreenRect(const Rect& rect) {
  rect_ = rect;
}

void Contents::Invalidate() {
  if (parent_)
    parent_->Invalidate();
}
