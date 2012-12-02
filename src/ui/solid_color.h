#ifndef UI_SOLID_COLOR_H_
#define UI_SOLID_COLOR_H_

#include "ui/contents.h"

class SolidColor : public Contents {
 public:
  SolidColor(Gwen::Color color) : color_(color) {} 
  virtual ~SolidColor() {}

  virtual void Render(
      const Skin& skin, Gwen::Renderer::Base* renderer) OVERRIDE {
    renderer->SetDrawColor(color_);
    const Rect& rect = GetScreenRect();
    renderer->DrawFilledRect(Gwen::Rect(rect.x, rect.y, rect.w, rect.h));
  }

 private:
  Gwen::Color color_;

  DISALLOW_COPY_AND_ASSIGN(SolidColor);
};

#endif  // UI_SOLID_COLOR_H_
