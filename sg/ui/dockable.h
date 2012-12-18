// Copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef SG_UI_DOCKABLE_H_
#define SG_UI_DOCKABLE_H_

#include "base/basictypes.h"
#include "base/compiler_specific.h"
#include "base/memory/scoped_ptr.h"
#include "sg/basic_geometric_types.h"
#include "sg/ui/drag_setup.h"
#include "sg/ui/input.h"

class DockingSplitContainer;
class Draggable;
class Renderer;
class Workspace;

class Dockable : public InputHandler {
 public:
  Dockable();
  virtual ~Dockable();

  virtual bool CanUndock() const { return false; }
  virtual bool IsContainer() const { return false; }
  virtual DockingSplitContainer* AsDockingSplitContainer();

  virtual void SetScreenRect(const Rect& rect);
  const Rect& GetScreenRect() const;

  void set_parent(DockingSplitContainer* parent) { parent_ = parent; }
  DockingSplitContainer* parent() { return parent_; }

  virtual void Render(Renderer* renderer) {}
  virtual void Invalidate();
  virtual bool CouldStartDrag(DragSetup* drag_setup) {
    return false;
  }

  virtual int X() const { return GetScreenRect().x; }
  virtual int Y() const { return GetScreenRect().y; }
  virtual int Width() const { return GetScreenRect().w; }
  virtual int Height() const { return GetScreenRect().h; }
  Rect GetClientRect() {
    return Rect(0, 0, GetScreenRect().w, GetScreenRect().h);
  }
  Point ToClient(const Point& point) {
    return Point(point.x - GetScreenRect().x, point.y - GetScreenRect().y);
  }

  // Find the Dockable in the tree that's top-most in the stacking order (or
  // equivalently lowest in the tree).
  virtual Dockable* FindTopMostUnderPoint(const Point& point);

  // Default implementation of InputHandler.
  virtual bool NotifyMouseMoved(
      int x, int y, int dx, int dy, const InputModifiers& modifiers) OVERRIDE {
    return false;
  }
  virtual bool NotifyMouseWheel(
      int delta, const InputModifiers& modifiers) OVERRIDE {
    return false;
  }
  virtual bool NotifyMouseButton(
      int index, bool down, const InputModifiers& modifiers) OVERRIDE {
    return false;
  }
  virtual bool NotifyKey(
      InputKey key, bool down, const InputModifiers& modifiers) OVERRIDE {
    return false;
  }
  virtual bool WantMouseEvents() { return false; }
  virtual bool WantKeyEvents() { return false; }

 private:
  DockingSplitContainer* parent_;
  Rect rect_;

  DISALLOW_COPY_AND_ASSIGN(Dockable);
};

#endif  // SG_UI_DOCKABLE_H_
