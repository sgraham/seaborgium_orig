// Copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "sg/ui/docking_resizer.h"

DockingResizer::DockingResizer(DockingSplitContainer* resizing)
    : resizing_(resizing) {
}

DockingResizer::~DockingResizer() {
}

void DockingResizer::Drag(const Point& point) {
}
