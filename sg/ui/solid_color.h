// Copyright 2012 The Chromium Authors.

#ifndef SG_UI_SOLID_COLOR_H_
#define SG_UI_SOLID_COLOR_H_

#include "ui/contents.h"

class SolidColor : public Contents {
 public:
  SolidColor(const Skin& skin, const Color& color)
      : Contents(skin),
        color_(color) {
  }
  virtual ~SolidColor() {}

  virtual void Render(Renderer* renderer) OVERRIDE {
    renderer->SetDrawColor(color_);
    renderer->DrawFilledRect(GetClientRect());
  }

 private:
  Color color_;

  DISALLOW_COPY_AND_ASSIGN(SolidColor);
};

#endif  // SG_UI_SOLID_COLOR_H_
