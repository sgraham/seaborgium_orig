// Copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef SG_UI_DOCKING_RESIZER_H_
#define SG_UI_DOCKING_RESIZER_H_

#include "sg/basic_geometric_types.h"

class DockingSplitContainer;

class DockingResizer {
 public:
  DockingResizer(DockingSplitContainer* resizing);
  virtual ~DockingResizer();

  void Drag(const Point& point);
  void CancelDrag();

  const Point& GetInitialLocationForTest() const { return initial_location_; }

 private:
  int ParentSize();
  int ComponentForDirection(const Point& point);

  DockingSplitContainer* resizing_;
  // Relative to parent.
  Point initial_location_;
};

#endif  // SG_UI_DOCKING_RESIZER_H_
