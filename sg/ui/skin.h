// Copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef SG_UI_SKIN_H_
#define SG_UI_SKIN_H_

#include "Gwen/Structures.h"

class ColorScheme {
 public:
  // TODO(config): Configuration from .json or something.
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

  const Gwen::Color& comment() const { return comment_; }
  const Gwen::Color& comment_preprocessor() const {
    return comment_preprocessor_;
  }
  const Gwen::Color& error() const { return error_; }
  const Gwen::Color& keyword() const { return keyword_; }
  const Gwen::Color& keyword_type() const { return keyword_type_; }
  const Gwen::Color& literal_number() const { return literal_number_; }
  const Gwen::Color& literal_string() const { return literal_string_; }
  const Gwen::Color& klass() const { return klass_; }
  const Gwen::Color& op() const { return op_; }

  const Gwen::Color& margin() const { return margin_; }
  const Gwen::Color& margin_text() const { return margin_text_; }

 private:
  Gwen::Color border_;
  Gwen::Color background_;
  Gwen::Color text_;
  Gwen::Color title_bar_active_;
  Gwen::Color title_bar_text_active_;
  Gwen::Color title_bar_inactive_;
  Gwen::Color title_bar_text_inactive_;

  Gwen::Color comment_;
  Gwen::Color comment_preprocessor_;
  Gwen::Color error_;
  Gwen::Color keyword_;
  Gwen::Color keyword_type_;
  Gwen::Color literal_number_;
  Gwen::Color literal_string_;
  Gwen::Color klass_;
  Gwen::Color op_;

  Gwen::Color margin_;
  Gwen::Color margin_text_;
};

class Skin {
 public:
  Skin();

  const ColorScheme& GetColorScheme() const { return color_scheme_; }

  int title_bar_size() const { return title_bar_size_; }
  int border_size() const { return border_size_; }
  int status_bar_size() const { return status_bar_size_; }

 private:
  ColorScheme color_scheme_;

  int title_bar_size_;
  int border_size_;
  int status_bar_size_;
};

#endif  // SG_UI_SKIN_H_
