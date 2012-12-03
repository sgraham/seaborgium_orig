// Copyright 2012 The Chromium Authors.

#ifndef SG_UI_SOLID_COLOR_H_
#define SG_UI_SOLID_COLOR_H_

#include "ui/contents.h"

class SolidColor : public Contents {
 public:
  SolidColor(const Skin& skin, Gwen::Color color)
      : Contents(skin),
        color_(color) {
  }
  virtual ~SolidColor() {}

  virtual void Render(Gwen::Renderer::Base* renderer) OVERRIDE {
    renderer->SetDrawColor(color_);
    const Rect& rect = GetScreenRect();
    renderer->DrawFilledRect(Gwen::Rect(rect.x, rect.y, rect.w, rect.h));
  }

 private:
  Gwen::Color color_;

  DISALLOW_COPY_AND_ASSIGN(SolidColor);
};

#endif  // SG_UI_SOLID_COLOR_H_
