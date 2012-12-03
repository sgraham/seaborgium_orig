// Copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef SG_RENDERER_WIN_H_
#define SG_RENDERER_WIN_H_

#include "sg/renderer.h"

class RendererWin : public Renderer {
 public:
  explicit RendererWin(...);

  // Implementation of Renderer:
  virtual void SetColor(const Color& color) OVERRIDE;

  virtual void LoadFont(Font* font) OVERRIDE;
  virtual void FreeFont(Font* font) OVERRIDE;
  virtual void DrawText(
      Font* font, const Point& pos, const string16& text) OVERRIDE;
  virtual Point MeasureText(Font* font, const string16& text) OVERRIDE;

  virtual void DrawFilledRect(const Rect& rect) OVERRIDE;
};

#endif  // SG_RENDERER_WIN_H_
