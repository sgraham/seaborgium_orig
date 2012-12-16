// Copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef SG_UI_DOCKABLE_H_
#define SG_UI_DOCKABLE_H_

#include "sg/basic_geometric_types.h"

class DockingSplitContainer;

class Dockable {
 public:
  Dockable();
  virtual ~Dockable();

  virtual bool CanUndock() const { return false; }
  virtual bool IsContainer() const { return false; }
  virtual DockingSplitContainer* AsDockingSplitContainer();

  virtual void SetScreenRect(const Rect& rect);
  const Rect& GetScreenRect() const;

  void set_parent(DockingSplitContainer* parent) { parent_ = parent; }
  DockingSplitContainer* get_parent() { return parent_; }

 private:
  DockingSplitContainer* parent_;
  Rect rect_;
};

#endif  // SG_UI_DOCKABLE_H_
