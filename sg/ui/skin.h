// Copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef SG_UI_SKIN_H_
#define SG_UI_SKIN_H_

#include "sg/basic_geometric_types.h"
#include "sg/render/font.h"
#include "sg/render/texture.h"

class ColorScheme {
 public:
  // TODO(config): Configuration from .json or something.
  ColorScheme();
  const Color& border() const { return border_; }
  const Color& background() const { return background_; }
  const Color& text() const { return text_; }
  const Color& title_bar_active() const { return title_bar_active_; }
  const Color& title_bar_text_active() const { return title_bar_text_active_; }
  const Color& title_bar_inactive() const { return title_bar_inactive_; }
  const Color& title_bar_text_inactive() const {
    return title_bar_text_inactive_;
  }

  const Color& comment() const { return comment_; }
  const Color& comment_preprocessor() const { return comment_preprocessor_; }
  const Color& error() const { return error_; }
  const Color& keyword() const { return keyword_; }
  const Color& keyword_type() const { return keyword_type_; }
  const Color& literal_number() const { return literal_number_; }
  const Color& literal_string() const { return literal_string_; }
  const Color& klass() const { return klass_; }
  const Color& op() const { return op_; }

  const Color& margin() const { return margin_; }
  const Color& margin_text() const { return margin_text_; }

  const Color& pc_indicator() const { return pc_indicator_; }

 private:
  Color border_;
  Color background_;
  Color text_;
  Color title_bar_active_;
  Color title_bar_text_active_;
  Color title_bar_inactive_;
  Color title_bar_text_inactive_;

  Color comment_;
  Color comment_preprocessor_;
  Color error_;
  Color keyword_;
  Color keyword_type_;
  Color literal_number_;
  Color literal_string_;
  Color klass_;
  Color op_;

  Color margin_;
  Color margin_text_;

  Color pc_indicator_;
};

class Skin {
 public:
  Skin();

  const ColorScheme& GetColorScheme() const { return color_scheme_; }

  int title_bar_size() const { return title_bar_size_; }
  int border_size() const { return border_size_; }
  int status_bar_size() const { return status_bar_size_; }
  int text_line_height() const { return text_line_height_; }

  const Font* mono_font() const { return &mono_font_; }
  const Font* ui_font() const { return &ui_font_; }

  const Texture* pc_indicator_texture() const { return &pc_indicator_texture_; }
  const Texture* breakpoint_texture() const { return &breakpoint_texture_; }
  const Texture* dock_top_texture() const { return &dock_top_texture_; }
  const Texture* dock_left_texture() const { return &dock_left_texture_; }
  const Texture* dock_right_texture() const { return &dock_right_texture_; }
  const Texture* dock_bottom_texture() const { return &dock_bottom_texture_; }
  const Texture* vscrollbar_top_texture() const {
    return &vscrollbar_top_texture_;
  }
  const Texture* vscrollbar_middle_texture() const {
    return &vscrollbar_middle_texture_;
  }
  const Texture* vscrollbar_bottom_texture() const {
    return &vscrollbar_bottom_texture_;
  }

  static const Skin& current();
  static void EnsureTexturesLoaded(Renderer* renderer);

 private:
  ColorScheme color_scheme_;

  int title_bar_size_;
  int border_size_;
  int status_bar_size_;
  int text_line_height_;

  Font mono_font_;
  Font ui_font_;

  Texture pc_indicator_texture_;
  Texture breakpoint_texture_;
  Texture dock_top_texture_;
  Texture dock_left_texture_;
  Texture dock_right_texture_;
  Texture dock_bottom_texture_;
  Texture vscrollbar_top_texture_;
  Texture vscrollbar_middle_texture_;
  Texture vscrollbar_bottom_texture_;
};

#endif  // SG_UI_SKIN_H_
