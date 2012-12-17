// Copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "sg/ui/skin.h"

#include "sg/render/renderer.h"

// Temp manual config; Solarized Dark
namespace {

Color kBase03(0, 43, 54);
Color kBase02(7, 54, 66);
Color kBase01(88, 110, 117);
Color kBase00(101, 123, 131);
Color kBase0(131, 148, 150);
Color kBase1(147, 161, 161);
Color kBase2(238, 232, 213);
Color kBase3(253, 246, 227);
Color kYellow(181, 137, 0);
Color kOrange(203, 75, 22);
Color kRed(220, 50, 47);
Color kMagenta(211, 54, 130);
Color kViolet(108, 113, 196);
Color kBlue(38, 139, 210);
Color kCyan(42, 161, 152);
Color kGreen(133, 153, 0);

Skin g_skin;

}  // namespace

ColorScheme::ColorScheme()
  : border_(0, 12, 16),
    background_(kBase03),
    text_(kBase0),
    title_bar_active_(kBase00),
    title_bar_text_active_(kBase3),
    title_bar_inactive_(kBase02),
    title_bar_text_inactive_(kBase1),
    comment_(kBase01),
    comment_preprocessor_(kOrange),
    error_(kRed),
    keyword_(kGreen),
    keyword_type_(kYellow),
    literal_number_(kCyan),
    literal_string_(kViolet),
    klass_(kBlue),
    op_(kGreen),
    margin_(kBase02),
    margin_text_(kBase0),
    pc_indicator_(kYellow) {
}

Skin::Skin()
    : title_bar_size_(19),
      border_size_(3),
      status_bar_size_(26),
      text_line_height_(17) {
  mono_font_.facename = L"Consolas";
  mono_font_.size = 13.f;
  ui_font_.facename = L"Segoe UI";
  ui_font_.size = 12.f;
  pc_indicator_texture_.name = L"art/pc-location.png";
  breakpoint_texture_.name = L"art/breakpoint.png";
  dock_top_texture_.name = L"art/dock-indicator-top.png";
  dock_left_texture_.name = L"art/dock-indicator-left.png";
  dock_right_texture_.name = L"art/dock-indicator-right.png";
  dock_bottom_texture_.name = L"art/dock-indicator-bottom.png";
  vscrollbar_top_texture_.name = L"art/scrollbar-top.png";
  vscrollbar_middle_texture_.name = L"art/scrollbar-middle.png";
  vscrollbar_bottom_texture_.name = L"art/scrollbar-bottom.png";
}

// static
const Skin& Skin::current() {
  return g_skin;
}

// static
void Skin::EnsureTexturesLoaded(Renderer* renderer) {
  if (g_skin.pc_indicator_texture_.data)
    return;

  renderer->LoadTexture(&g_skin.pc_indicator_texture_);
  renderer->LoadTexture(&g_skin.breakpoint_texture_);
  renderer->LoadTexture(&g_skin.dock_top_texture_);
  renderer->LoadTexture(&g_skin.dock_left_texture_);
  renderer->LoadTexture(&g_skin.dock_right_texture_);
  renderer->LoadTexture(&g_skin.dock_bottom_texture_);
  renderer->LoadTexture(&g_skin.vscrollbar_top_texture_);
  renderer->LoadTexture(&g_skin.vscrollbar_middle_texture_);
  renderer->LoadTexture(&g_skin.vscrollbar_bottom_texture_);
}
