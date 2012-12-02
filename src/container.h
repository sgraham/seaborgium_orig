#ifndef CONTAINER_H_
#define CONTAINER_H_

#include "base/compiler_specific.h"
#include "base/logging.h"
#include "base/string_util.h"
#include "base/string16.h"
#include "Gwen/Structures.h"

class Rect {
 public:
  Rect() : x(0), y(0), w(-1), h(-1) {}
  Rect(int x, int y, int w, int h) : x(x), y(y), w(w), h(h) {}
  Rect Expand(const Rect& by) const {
    return Rect(x - by.x, y - by.y, w + by.x + by.w, h + by.y + by.w);
  }
  Rect Contract(const Rect& by) const {
    return Rect(x + by.x, y + by.y, w - by.x - by.w, h - by.y - by.h);
  }
  int x, y, w, h;
};

class Contents {
 public:
  Contents() : parent_(NULL) {}
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

  virtual const Rect& GetScreenRect() const { return rect_; }

 private:
  Contents* parent_;
  Rect rect_;

  DISALLOW_COPY_AND_ASSIGN(Contents);
};

class SolidColor : public Contents {
 public:
  SolidColor(Gwen::Color color) : color_(color) {} 
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

const int kTitleBarSize = 20;
const int kBorderSize = 5;
const Rect kInnerBorderSize(
    kBorderSize, kBorderSize + kTitleBarSize, kBorderSize, kBorderSize);
const Rect kOuterBorderSize(
    kBorderSize, kBorderSize, kBorderSize, kBorderSize);

const Gwen::Color kBorderColor = Gwen::Colors::Red;
const Gwen::Color kTitleColor = Gwen::Colors::Green;
const Gwen::Color kTitleBarTextColor = Gwen::Colors::Black;
Gwen::Font kUIFont(L"Consolas", 13.f);

class Container : public Contents {
 public:
  Container() : mode_(SplitHorizontal) {
  }
  virtual ~Container() {}

  enum Mode {
    SplitHorizontal,
    SplitVertical,
    Tabbed,
  };

  void SetMode(Mode mode) {
    mode_ = mode;
  }

  virtual void Render(Gwen::Renderer::Base* renderer) OVERRIDE {
    PropagateSizeChanges();
    RenderChildren(renderer);
    RenderBorders(renderer);
  }

  virtual void AddChild(Contents* contents) {
    // Scale existing children by (N-1)/N of space (where N includes addition).
    double new_count = static_cast<double>(children_.size() + 1);
    double scale = static_cast<double>(children_.size()) / new_count;
    for (size_t i = 0; i < children_.size(); ++i) {
      children_[i].fraction *= scale;
    }
    ChildData addition;
    addition.contents = contents;
    addition.fraction = 1.0;
    contents->SetParent(this);
    children_.push_back(addition);
  }

 private:
  struct ChildData {
    Contents* contents;
    double fraction;
  };
  std::vector<ChildData> children_;

  // Recalculate the screen rect for each of our children (assuming our |rect_|
  // is already up to date here).
  void PropagateSizeChanges() {
    if (children_.size() == 0)
      return;
    Rect rect = GetScreenRect();
    if (GetParent() == NULL)
      rect = rect.Contract(kOuterBorderSize);
    if (children_.size() == 1) {
      children_[0].contents->SetScreenRect(rect);
      return;
    }
    DCHECK(mode_ == SplitHorizontal || mode_ == SplitVertical) << "TODO";
    int remaining = (mode_ == SplitHorizontal ? rect.w : rect.h) -
        kBorderSize * (children_.size() - 1);
    double current = mode_ == SplitHorizontal ? rect.x : rect.y;
    double last_fraction = 0.0;
    // TODO: This might have some rounding problems.
    for (size_t i = 0; i < children_.size(); ++i) {
      Rect result = rect; // For x/w or y/h that isn't changed below.
      double for_child;
      if (mode_ == SplitHorizontal) {
        result.x = (int)current;
        for_child = (children_[i].fraction - last_fraction) * remaining;
        result.w = for_child;
      } else {
        result.y = (int)current;
        for_child = (children_[i].fraction - last_fraction) * remaining;
        result.h = for_child;
      }
      children_[i].contents->SetScreenRect(result);
      current += for_child + kBorderSize;
      last_fraction = children_[i].fraction;
    }
    DCHECK(last_fraction == 1.0);
  }

  void RenderChildren(Gwen::Renderer::Base* renderer) {
    for (size_t i = 0; i < children_.size(); ++i) {
      children_[i].contents->Render(renderer);
    }
  }

  void RenderBorders(Gwen::Renderer::Base* renderer) {
    /*
    RenderChildBorders(renderer);
    RenderFrame(renderer, GetScreenRect());
    */
  }

  void RenderChildBorders(Gwen::Renderer::Base* renderer) {
    if (children_.size() != 1)
      return;
    Rect rect = children_[0].contents->GetScreenRect().Expand(kInnerBorderSize);

    // Borders and splits. The between children will overlap.
    RenderFrame(renderer, rect);

    // Title bar.
    renderer->SetDrawColor(kTitleColor);
    renderer->DrawFilledRect(
        Gwen::Rect(rect.x + kBorderSize, rect.y + kBorderSize,
             rect.w - kBorderSize - kBorderSize, kTitleBarSize));
      // TODO: Pass rect through.
      renderer->SetDrawColor(kTitleBarTextColor);
      renderer->RenderText(
          &kUIFont,
          Gwen::Point(rect.x + kBorderSize, rect.y + kBorderSize),
          L"Title; TODO");
  }

  void RenderFrame(Gwen::Renderer::Base* renderer, const Rect& rect) {
    renderer->SetDrawColor(kBorderColor);
    renderer->DrawFilledRect(
        Gwen::Rect(rect.x, rect.y, rect.w, kBorderSize));
    renderer->DrawFilledRect(
        Gwen::Rect(rect.x, rect.y, kBorderSize, rect.h));
    renderer->DrawFilledRect(Gwen::Rect(
        rect.x + rect.w - kBorderSize, rect.y, kBorderSize, rect.h));
    renderer->DrawFilledRect(Gwen::Rect(
        rect.x, rect.y + rect.h - kBorderSize, rect.w, kBorderSize));
  }

  Mode mode_;

  DISALLOW_COPY_AND_ASSIGN(Container);
};

#endif  // CONTAINER_H_

