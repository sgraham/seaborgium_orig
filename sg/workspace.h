// Copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef SG_WORKSPACE_H_
#define SG_WORKSPACE_H_

#include "sg/ui/container.h"
#include "sg/ui/skin.h"

class Container;

class Workspace : public Container {
 public:
  Workspace();
  virtual ~Workspace();

 private:
  Skin skin_;
};

#endif  // SG_WORKSPACE_H_
