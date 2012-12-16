// Copyright 2012 The Chromium Authors.

#ifndef SG_UI_SOLID_COLOR_H_
#define SG_UI_SOLID_COLOR_H_

#include "sg/render/renderer.h"
#include "sg/ui/dockable.h"

class SolidColor : public Dockable {
 public:
  SolidColor(const Color& color)
      : color_(color) {
  }
  virtual ~SolidColor() {}

  virtual void Render(Renderer* renderer, const Skin& skin) OVERRIDE {
    renderer->SetDrawColor(color_);
    renderer->DrawFilledRect(GetClientRect());
  }

 private:
  Color color_;

  DISALLOW_COPY_AND_ASSIGN(SolidColor);
};

#endif  // SG_UI_SOLID_COLOR_H_
