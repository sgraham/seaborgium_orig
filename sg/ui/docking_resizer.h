// Copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef SG_UI_DOCKING_RESIZER_H_
#define SG_UI_DOCKING_RESIZER_H_

#include "sg/basex/compiler_specific.h"
#include "sg/basic_geometric_types.h"
#include "sg/ui/draggable.h"

class DockingSplitContainer;

class DockingResizer : public Draggable {
 public:
  explicit DockingResizer(DockingSplitContainer* resizing);
  virtual ~DockingResizer();

  virtual void Drag(const Point& screen_point) OVERRIDE;
  virtual void CancelDrag() OVERRIDE;
  virtual void Render(Renderer* renderer) OVERRIDE;

  const Point& GetInitialLocationForTest() const { return initial_location_; }

 private:
  int ParentSize();
  int ComponentForDirection(const Point& point);

  DockingSplitContainer* resizing_;
  // Relative to parent.
  Point initial_location_;
};

#endif  // SG_UI_DOCKING_RESIZER_H_
