// Copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef SG_RENDERER_H_
#define SG_RENDERER_H_

#include "sg/font.h"

class Renderer {
 public:
  virtual ~Renderer() {}

  // TODO(rendering) All temporary during porting from Gwen.

  virtual void SetColor(const Color& color) = 0;

  virtual void LoadFont(Font* font) = 0;
  virtual void FreeFont(Font* font) = 0;
  virtual void DrawText(Font* font, const Point& pos, const string16& text) = 0;
  virtual Point MeasureText(Font* font, const string16& text) = 0;

  virtual void DrawFilledRect(const Rect& rect) = 0;

  void SetRenderOffset(const Point& point) { render_offset_ = offset; }
  void AddRenderOffset(const Point& offset) {
    render_offset_.x = offset.x;
    render_offset_.y = offset.y;
  }
  const Point& GetRenderOffset() const { return render_offset_; }

  void Translate(int* x, int* y) {
    *x += render_offset_.x;
    *y += render_offset_.y;
  }

 protected:
  Point render_offset_;

 private:
  DISALLOW_COPY_AND_ASSIGN(Renderer);
};

Renderer* CreateRenderer();

#endif  // SG_RENDERER_H_
