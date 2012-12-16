// Copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef SG_UI_DOCKING_RESIZER_H_
#define SG_UI_DOCKING_RESIZER_H_

class DockingSplitContainer;
class Point;

class DockingResizer {
 public:
  DockingResizer(DockingSplitContainer* resizing);
  virtual ~DockingResizer();

  void Drag(const Point& point);

 private:
  DockingSplitContainer* resizing_;
};

#endif  // SG_UI_DOCKING_RESIZER_H_
