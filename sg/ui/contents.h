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
  explicit Contents(const Skin& skin);
  virtual ~Contents();

  virtual void Render(Gwen::Renderer::Base* renderer) = 0;

  virtual void SetParent(Contents* parent);
  virtual Contents* GetParent() const;

  virtual void SetScreenRect(const Rect& rect);

  virtual void Invalidate();

  virtual int X() const { return rect_.x; }
  virtual int Y() const { return rect_.y; }
  virtual int Width() const { return rect_.w; }
  virtual int Height() const { return rect_.h; }

  virtual const Rect& GetScreenRect() const { return rect_; }
  virtual Rect GetClientRect() const {
    return Rect(0, 0, rect_.w, rect_.h);
  }

  virtual bool IsLeaf() const { return true; }

  virtual const Skin& GetSkin() const { return skin_; }

 private:
  Contents* parent_;
  const Skin& skin_;
  Rect rect_;

  DISALLOW_COPY_AND_ASSIGN(Contents);
};

#endif  // SG_UI_CONTENTS_H_
