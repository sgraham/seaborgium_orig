// Copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef SG_UI_CONTENTS_H_
#define SG_UI_CONTENTS_H_

#include "base/basictypes.h"
#include "base/compiler_specific.h"
#include "Gwen/BaseRender.h"
#include "sg/ui/base_types.h"
class Skin;

class Contents {
 public:
  explicit Contents(const Skin& skin) : parent_(NULL), skin_(skin) {}
  virtual ~Contents() {}

  virtual void SetParent(Contents* parent) {
    parent_ = parent;
  }
  virtual Contents* GetParent() const {
    return parent_;
  }

  virtual void Render(Gwen::Renderer::Base* renderer) = 0;

  virtual void SetScreenRect(const Rect& rect) {
    rect_ = rect;
  }

  virtual int X() const { return rect_.x; }
  virtual int Y() const { return rect_.y; }
  virtual int Width() const { return rect_.w; }
  virtual int Height() const { return rect_.h; }

  virtual const Rect& GetScreenRect() const { return rect_; }

  virtual bool CanHoldChildren() const { return false; }

  virtual const Skin& GetSkin() const { return skin_; }

 private:
  Contents* parent_;
  const Skin& skin_;
  Rect rect_;

  DISALLOW_COPY_AND_ASSIGN(Contents);
};

#endif  // SG_UI_CONTENTS_H_
