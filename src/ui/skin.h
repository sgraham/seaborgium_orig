#ifndef SKIN_H_
#define SKIN_H_

#include "Gwen/Structures.h"

class ColorScheme {
 public:
  // TODO: Configuration from .json or something.
  ColorScheme();
  const Gwen::Color& border() const { return border_; }
  const Gwen::Color& background() const { return background_; }
  const Gwen::Color& text() const { return text_; }
  const Gwen::Color& title_bar_active() const { return title_bar_active_; }
  const Gwen::Color& title_bar_text_active() const {
    return title_bar_text_active_;
  }
  const Gwen::Color& title_bar_inactive() const { return title_bar_inactive_; }
  const Gwen::Color& title_bar_text_inactive() const {
    return title_bar_text_inactive_;
  }

 private:
  Gwen::Color border_;
  Gwen::Color background_;
  Gwen::Color text_;
  Gwen::Color title_bar_active_;
  Gwen::Color title_bar_text_active_;
  Gwen::Color title_bar_inactive_;
  Gwen::Color title_bar_text_inactive_;
};

class Skin {
 public:
  Skin();

  const ColorScheme& GetColorScheme() const { return color_scheme_; }

  int title_bar_size() const { return title_bar_size_; }
  int border_size() const { return border_size_; }

 private:
  ColorScheme color_scheme_;

  int title_bar_size_;
  int border_size_;
};

#endif  // SKIN_H_
