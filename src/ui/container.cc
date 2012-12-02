#include "ui/container.h"

#include "base/logging.h"
#include "Gwen/Font.h"
#include "skin.h"
#include "ui/focus.h"

// TODO
Gwen::Font kUIFont(L"Segoe UI", 12.f);

Container::Container() : mode_(SplitHorizontal) {
}

Container::~Container() {
}

void Container::Render(
    const Skin& skin, Gwen::Renderer::Base* renderer) {
  PropagateSizeChanges(skin);
  RenderChildren(skin, renderer);
  RenderBorders(skin, renderer);
}

void Container::AddChild(Contents* contents, const string16& title) {
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

void Container::SetFraction(Contents* contents, double fraction) {
  // TODO: Verify legal, or renormalize.
  // TODO: Maybe should be stored in child?
  for (size_t i = 0; i < children_.size(); ++i) {
    if (children_[i].contents == contents) {
      children_[i].fraction = fraction;
      return;
    }
  }
  NOTREACHED() << "Child not found.";
}

void Container::PropagateSizeChanges(const Skin& skin) {
  if (children_.size() == 0)
    return;
  Rect rect = GetScreenRect();
  if (GetParent() == NULL) {
    Rect outer_border_size(
        skin.border_size(), skin.border_size(),
        skin.border_size(), skin.border_size());
    rect = rect.Contract(outer_border_size);
  }
  if (children_.size() == 1) {
    ChildData* child = &children_[0];
    if (!child->contents->CanHoldChildren()) {
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
  // TODO: This might have some rounding problems.
  for (size_t i = 0; i < children_.size(); ++i) {
    Rect result = rect; // For x/w or y/h that isn't changed below.
    ChildData* child = &children_[i];
    double for_child;
    if (mode_ == SplitHorizontal) {
      result.x = (int)current;
      for_child = (child->fraction - last_fraction) * remaining;
      result.w = for_child;
    } else {
      result.y = (int)current;
      for_child = (child->fraction - last_fraction) * remaining;
      result.h = for_child;
    }
    child->contents->SetScreenRect(result);
    current += for_child + skin.border_size();
    last_fraction = child->fraction;
  }
  DCHECK(last_fraction == 1.0);
}

void Container::RenderChildren(const Skin& skin, Gwen::Renderer::Base* renderer) {
  for (size_t i = 0; i < children_.size(); ++i) {
    children_[i].contents->Render(skin, renderer);
  }
}

void Container::RenderBorders(const Skin& skin, Gwen::Renderer::Base* renderer) {
  // TODO: This (and the resize code) is wrong.
  if (GetParent() == NULL)
    RenderFrame(skin, renderer, GetScreenRect());
  Rect title_border_size(
      skin.border_size(), skin.border_size() + skin.title_bar_size(),
      skin.border_size(), skin.border_size());
  if (children_.size() == 1) {
    ChildData* child = &children_[0];
    if (!child->contents->CanHoldChildren()) {
      Rect rect = child->contents->GetScreenRect().Expand(title_border_size);
      RenderFrame(skin, renderer, rect);

      // And title bar.
      bool is_focused = GetFocusedContents() == child->contents;
      Gwen::Color title_bar_background = is_focused ?
          skin.GetColorScheme().title_bar_active() :
          skin.GetColorScheme().title_bar_inactive();
      Gwen::Color title_bar_text = is_focused ?
          skin.GetColorScheme().title_bar_text_active() :
          skin.GetColorScheme().title_bar_text_inactive();
      renderer->SetDrawColor(title_bar_background);
      renderer->DrawFilledRect(
          Gwen::Rect(rect.x + skin.border_size(), rect.y + skin.border_size(),
                rect.w - skin.border_size() * 2, skin.title_bar_size()));

      // TODO: Pass rect through to renderer.
      renderer->SetDrawColor(title_bar_text);
      renderer->RenderText(
          &kUIFont,
          Gwen::Point(rect.x + skin.border_size() + 3,
                      rect.y + skin.border_size()),
          children_[0].title);
    }
  }
}

void Container::RenderFrame(
    const Skin& skin, Gwen::Renderer::Base* renderer, const Rect& rect) {
  renderer->SetDrawColor(skin.GetColorScheme().border());
  renderer->DrawFilledRect(
      Gwen::Rect(rect.x, rect.y, rect.w, skin.border_size()));
  renderer->DrawFilledRect(
      Gwen::Rect(rect.x, rect.y, skin.border_size(), rect.h));
  renderer->DrawFilledRect(Gwen::Rect(
      rect.x + rect.w - skin.border_size(),
      rect.y, skin.border_size(), rect.h));
  renderer->DrawFilledRect(Gwen::Rect(
      rect.x, rect.y + rect.h - skin.border_size(),
      rect.w, skin.border_size()));
}
