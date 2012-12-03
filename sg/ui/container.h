// Copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef SG_UI_CONTAINER_H_
#define SG_UI_CONTAINER_H_

#include <vector>

#include "base/string16.h"
#include "sg/ui/contents.h"

class Container : public Contents {
 public:
  explicit Container(const Skin& skin);
  virtual ~Container();

  enum Mode {
    SplitHorizontal,
    SplitVertical,
    Tabbed,
  };

  void SetMode(Mode mode) {
    mode_ = mode;
  }

  virtual void Render(Gwen::Renderer::Base* renderer) OVERRIDE;
  virtual void AddChild(Contents* contents, const string16& title);
  virtual void AddChild(Contents* contents);
  virtual void SetFraction(Contents* contents, double fraction);
  virtual bool CanHoldChildren() const { return true; }
  virtual size_t ChildCount() const { return children_.size(); }
  virtual Contents* Child(size_t i) { return children_[i].contents; }

 private:
  // Recalculate the screen rect for each of our children (assuming our |rect_|
  // is already up to date here).
  void PropagateSizeChanges();
  void RenderChildren(Gwen::Renderer::Base* renderer);
  void RenderBorders(Gwen::Renderer::Base* renderer);
  void RenderFrame(Gwen::Renderer::Base* renderer, const Rect& rect);

  struct ChildData {
    Contents* contents;
    double fraction;
    string16 title;
  };
  std::vector<ChildData> children_;

  Mode mode_;

  DISALLOW_COPY_AND_ASSIGN(Container);
};

#endif  // SG_UI_CONTAINER_H_
