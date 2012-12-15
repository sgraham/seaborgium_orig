// Copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "sg/ui/container.h"

#include "base/logging.h"
#include "sg/render/font.h"
#include "sg/ui/focus.h"
#include "sg/ui/skin.h"

// TODO(globals)
// TODO(rendering)
Font kUIFont(L"Segoe UI", 12.f);

Container::Container(const Skin& skin)
    : Contents(skin),
      mode_(SplitHorizontal) {
}

Container::~Container() {
  for (size_t i = 0; i < children_.size(); ++i) {
    delete children_[i].contents;
  }
}

void Container::Render(Renderer* renderer) {
  RenderChildren(renderer);
  RenderBorders(renderer);
  RenderHover(renderer);
}

void Container::SetScreenRect(const Rect& rect) {
  Contents::SetScreenRect(rect);
  DoStandardLayout();
}

void Container::AddChild(Contents* contents, const string16& title) {
  DCHECK(!contents->GetParent());
  // Scale existing children by (N-1)/N of space (where N includes addition).
  double new_count = static_cast<double>(children_.size() + 1);
  double scale = static_cast<double>(children_.size()) / new_count;
  for (size_t i = 0; i < children_.size(); ++i) {
    children_[i].fraction *= scale;
  }
  ChildData addition;
  addition.contents = contents;
  addition.fraction = 1.0;
  addition.title = title;
  contents->SetParent(this);
  children_.push_back(addition);
}

void Container::AddChild(Contents* contents) {
  AddChild(contents, L"<none>");
}

void Container::SetTitle(Contents* contents, const string16& title) {
  for (size_t i = 0; i < children_.size(); ++i) {
    if (children_[i].contents == contents) {
      children_[i].title = title;
      return;
    }
  }
  NOTREACHED();
}

void Container::SetFraction(Contents* contents, double fraction) {
  // TODO(testing): Verify legal, or renormalize.
  // TODO(scottmg): Maybe should be stored in child?
  for (size_t i = 0; i < children_.size(); ++i) {
    if (children_[i].contents == contents) {
      children_[i].fraction = fraction;
      return;
    }
  }
  NOTREACHED() << "Child not found.";
}

Contents* Container::FindContentsAt(const Point& screen_point) {
  for (size_t i = 0; i < children_.size(); ++i) {
    Contents* contents = children_[i].contents;
    if (contents->GetScreenRect().Contains(screen_point)) {
      if (contents->IsLeaf()) {
        return contents;
      } else {
        Container* as_container = reinterpret_cast<Container*>(contents);
        return as_container->FindContentsAt(screen_point);
      }
    }
  }
  return NULL;
}

void Container::DoStandardLayout() {
  if (children_.size() == 0)
    return;
  Rect rect = GetScreenRect();
  const Skin& skin = GetSkin();
  if (children_.size() == 1) {
    ChildData* child = &children_[0];
    if (child->contents->IsLeaf()) {
      rect.y += skin.title_bar_size();
      rect.h -= skin.title_bar_size();
    }
    child->contents->SetScreenRect(rect);
    return;
  }
  DCHECK(mode_ == SplitHorizontal || mode_ == SplitVertical) << "TODO";
  int remaining = (mode_ == SplitHorizontal ? rect.w : rect.h) -
      skin.border_size() * (children_.size() - 1);
  double current = mode_ == SplitHorizontal ? rect.x : rect.y;
  double last_fraction = 0.0;
  // TODO(scottmg): This has some rounding problems (so frame will be one
  // pixel too big sometimes).
  for (size_t i = 0; i < children_.size(); ++i) {
    Rect result = rect;  // For x/w or y/h that isn't changed below.
    ChildData* child = &children_[i];
    double for_child;
    if (mode_ == SplitHorizontal) {
      result.x = static_cast<int>(current);
      for_child = (child->fraction - last_fraction) * remaining;
      result.w = for_child;
    } else {
      result.y = static_cast<int>(current);
      for_child = (child->fraction - last_fraction) * remaining;
      result.h = for_child;
    }
    child->contents->SetScreenRect(result);
    current += for_child + skin.border_size();
    last_fraction = child->fraction;
  }
  DCHECK(last_fraction == 1.0);
}

void Container::RenderChildren(Renderer* renderer) {
  Point old_render_offset = renderer->GetRenderOffset();
  for (size_t i = 0; i < children_.size(); ++i) {
    const Rect& screen_rect = children_[i].contents->GetScreenRect();
    renderer->SetRenderOffset(Point(screen_rect.x, screen_rect.y));
    // TODO(rendering): Clip too.
    children_[i].contents->Render(renderer);
  }
  renderer->SetRenderOffset(old_render_offset);
}

Rect Container::GetTitleBorderSize() const {
  // TODO(rendering): Cache.
  const Skin& skin = GetSkin();
  return Rect(skin.border_size(), skin.border_size() + skin.title_bar_size(),
              skin.border_size(), skin.border_size());
}

void Container::RenderBorders(Renderer* renderer) {
  const Skin& skin = GetSkin();
  if (children_.size() == 1) {
    ChildData* child = &children_[0];
    if (child->contents->IsLeaf()) {
      Rect relative_rect =
          child->contents->GetScreenRect().RelativeTo(GetScreenRect());
      Rect rect = relative_rect.Expand(GetTitleBorderSize());
      RenderFrame(renderer, rect, skin.GetColorScheme().border());

      // And title bar.
      bool is_focused = GetFocusedContents() == child->contents;
      const Color& title_bar_background = is_focused ?
          skin.GetColorScheme().title_bar_active() :
          skin.GetColorScheme().title_bar_inactive();
      const Color& title_bar_text = is_focused ?
          skin.GetColorScheme().title_bar_text_active() :
          skin.GetColorScheme().title_bar_text_inactive();
      renderer->SetDrawColor(title_bar_background);
      renderer->DrawFilledRect(
          Rect(rect.x + skin.border_size(), rect.y + skin.border_size(),
               rect.w - skin.border_size() * 2, skin.title_bar_size()));

      // TODO(rendering): Pass rect through to renderer.
      renderer->SetDrawColor(title_bar_text);
      renderer->RenderText(
          &kUIFont,
          Point(rect.x + skin.border_size() + 3,
                rect.y + skin.border_size()),
          children_[0].title);
    }
  }
}

void Container::RenderFrame(
    Renderer* renderer, const Rect& rect, const Color& color) {
  renderer->SetDrawColor(color);
  const Skin& skin = GetSkin();
  renderer->DrawFilledRect(
      Rect(rect.x, rect.y, rect.w, skin.border_size()));
  renderer->DrawFilledRect(
      Rect(rect.x, rect.y, skin.border_size(), rect.h));
  renderer->DrawFilledRect(Rect(
      rect.x + rect.w - skin.border_size(),
      rect.y, skin.border_size(), rect.h));
  renderer->DrawFilledRect(Rect(
      rect.x, rect.y + rect.h - skin.border_size(),
      rect.w, skin.border_size()));
}

void Container::RenderHover(Renderer* renderer) {
  Contents* contents = GetHoveredContents();
  if (contents) {
    const Skin& skin = GetSkin();
    RenderFrame(renderer,
                contents->GetScreenRect()
                    .RelativeTo(GetScreenRect())
                    .Expand(GetTitleBorderSize()),
                skin.GetColorScheme().border_active());
  }
}
