// Copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "sg/ui/scroll_helper.h"

#include "base/logging.h"
#include "sg/render/texture.h"

namespace {

Texture g_vscrollbar_top;
Texture g_vscrollbar_bottom;
Texture g_vscrollbar_middle;
const int kFadeOutAfterTicks = 90;
const int kFadeOutOverTicks = 30;

}

ScrollHelper::ScrollHelper(const Skin& skin,
                           ScrollHelperDataProvider* data_provider,
                           int num_pixels_in_line)
    : y_pixel_scroll_(0),
      y_pixel_scroll_target_(0),
      // Start hidden.
      ticks_since_stopped_moving_(kFadeOutAfterTicks + kFadeOutOverTicks),
      num_pixels_in_line_(num_pixels_in_line),
      data_provider_(data_provider) {
  // TODO(rendering): skin for these.
  g_vscrollbar_top.name = L"art/scrollbar-top.png";
  g_vscrollbar_bottom.name = L"art/scrollbar-bottom.png";
  g_vscrollbar_middle.name = L"art/scrollbar-middle.png";
}

ScrollHelper::~ScrollHelper() {
}

bool ScrollHelper::Update() {
  float delta = (y_pixel_scroll_target_ - y_pixel_scroll_) * 0.2f;
  y_pixel_scroll_ += delta;
  if (fabs(delta) < 1) {
    y_pixel_scroll_ = y_pixel_scroll_target_;
    ticks_since_stopped_moving_++;
    return ticks_since_stopped_moving_ <
           (kFadeOutAfterTicks + kFadeOutOverTicks);
  }
  ticks_since_stopped_moving_ = 0;
  return true;
}

void ScrollHelper::RenderScrollIndicators(Renderer::Base* renderer) {
  // TODO(rendering)
  if (!g_vscrollbar_top.data) {
    renderer->LoadTexture(&g_vscrollbar_top);
    renderer->LoadTexture(&g_vscrollbar_bottom);
    renderer->LoadTexture(&g_vscrollbar_middle);
  }

  int scrollable_height = data_provider_->GetContentSize() +
                          data_provider_->GetScreenRect().h -
                          num_pixels_in_line_;
  Rect screen_rect = data_provider_->GetScreenRect();
  int visible_height = screen_rect.h;
  double fraction_visible = static_cast<double>(visible_height) /
                            static_cast<double>(scrollable_height);
  int scrollbar_height = static_cast<int>(visible_height * fraction_visible);

  double offset_fraction = static_cast<double>(y_pixel_scroll_) /
                           static_cast<double>(scrollable_height);
  int scrollbar_offset = static_cast<int>(visible_height * offset_fraction);

  double alpha = 1.0;
  if (ticks_since_stopped_moving_ >= kFadeOutAfterTicks) {
    alpha = 1.0 - static_cast<double>(
        ticks_since_stopped_moving_ - kFadeOutAfterTicks) / kFadeOutOverTicks;
  }

  int scrollbar_middle_height =
      scrollbar_height - g_vscrollbar_top.height - g_vscrollbar_bottom.height;
  int x = screen_rect.w - g_vscrollbar_middle.width;
  int y = scrollbar_offset;
  renderer->DrawTexturedRectAlpha(
      &g_vscrollbar_top,
      Rect(x, y, g_vscrollbar_top.width, g_vscrollbar_top.height),
      alpha,
      0, 0, 1, 1);
  y += g_vscrollbar_top.height;
  renderer->DrawTexturedRectAlpha(
      &g_vscrollbar_middle,
      Rect(x, y, g_vscrollbar_middle.width, scrollbar_middle_height),
      alpha,
      0, 0, 1, 1);
  y += scrollbar_middle_height;
  renderer->DrawTexturedRectAlpha(
      &g_vscrollbar_bottom,
      Rect(x, y, g_vscrollbar_bottom.width, g_vscrollbar_bottom.height),
      alpha,
      0, 0, 1, 1);
}

bool ScrollHelper::ClampScrollTarget() {
  y_pixel_scroll_target_ = std::max(0, y_pixel_scroll_target_);
  int largest_possible =
      data_provider_->GetContentSize() - num_pixels_in_line_;
  y_pixel_scroll_target_ = std::min(largest_possible, y_pixel_scroll_target_);
  return y_pixel_scroll_ != y_pixel_scroll_target_;
}

bool ScrollHelper::ScrollPixels(int delta) {
  y_pixel_scroll_target_ += delta;
  return ClampScrollTarget();
}

bool ScrollHelper::ScrollLines(int delta) {
  y_pixel_scroll_target_ += delta * num_pixels_in_line_;
  return ClampScrollTarget();
}

bool ScrollHelper::ScrollPages(int delta) {
  int screen_height = data_provider_->GetScreenRect().h;
  y_pixel_scroll_target_ += delta * screen_height - num_pixels_in_line_;
  return ClampScrollTarget();
}

bool ScrollHelper::ScrollToBeginning() {
  y_pixel_scroll_target_ = 0;
  return ClampScrollTarget();
}

bool ScrollHelper::ScrollToEnd() {
  y_pixel_scroll_target_ =
      data_provider_->GetContentSize() - num_pixels_in_line_;
  return ClampScrollTarget();
}
