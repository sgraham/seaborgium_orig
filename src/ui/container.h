#ifndef UI_CONTAINER_H_
#define UI_CONTAINER_H_

#include "base/string16.h"
#include "ui/contents.h"

#include <vector>

class Container : public Contents {
 public:
  Container();
  virtual ~Container();

  enum Mode {
    SplitHorizontal,
    SplitVertical,
    Tabbed,
  };

  void SetMode(Mode mode) {
    mode_ = mode;
  }

  virtual void Render(
      const Skin& skin, Gwen::Renderer::Base* renderer) OVERRIDE;
  virtual void AddChild(Contents* contents, const string16& title);
  virtual void AddChild(Contents* contents);
  virtual void SetFraction(Contents* contents, double fraction);
  virtual bool CanHoldChildren() const { return true; }
  virtual size_t ChildCount() const { return children_.size(); }
  virtual Contents* Child(size_t i) { return children_[i].contents; }

 private:
  // Recalculate the screen rect for each of our children (assuming our |rect_|
  // is already up to date here).
  void PropagateSizeChanges(const Skin& skin);
  void RenderChildren(const Skin& skin, Gwen::Renderer::Base* renderer);
  void RenderBorders(const Skin& skin, Gwen::Renderer::Base* renderer);
  void RenderFrame(
      const Skin& skin, Gwen::Renderer::Base* renderer, const Rect& rect);

  struct ChildData {
    Contents* contents;
    double fraction;
    string16 title;
  };
  std::vector<ChildData> children_;

  Mode mode_;

  DISALLOW_COPY_AND_ASSIGN(Container);
};

#endif  // UI_CONTAINER_H_
