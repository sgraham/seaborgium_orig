// Copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef SG_WORKSPACE_H_
#define SG_WORKSPACE_H_

#include "Gwen/Gwen.h"
#include "Gwen/Align.h"
#include "Gwen/Utility.h"
#include "ui/skin.h"

class Container;

class Workspace : public Gwen::Controls::Base {
 public:
  GWEN_CONTROL(Workspace, Gwen::Controls::Base);
  virtual ~Workspace();
  void Render(Gwen::Skin::Base* skin);

 private:
  Container* root_;
  Skin skin_;
};

#endif  // SG_WORKSPACE_H_
