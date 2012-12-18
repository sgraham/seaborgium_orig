// Copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "sg/ui/scroll_helper.h"

#include <algorithm>

#include "base/logging.h"
#include "sg/render/renderer.h"
#include "sg/render/texture.h"
#include "sg/ui/skin.h"

namespace {

const int kFadeOutAfterTicks = 90;
const int kFadeOutOverTicks = 30;

}  // namespace

ScrollHelper::ScrollHelper(ScrollHelperDataProvider* data_provider,
                           int num_pixels_in_line)
    : y_pixel_scroll_(0),
      y_pixel_scroll_target_(0),
      // Start hidden.
      ticks_since_stopped_moving_(kFadeOutAfterTicks + kFadeOutOverTicks),
      num_pixels_in_line_(num_pixels_in_line),
      data_provider_(data_provider) {
}

ScrollHelper::~ScrollHelper() {
}

bool ScrollHelper::Update() {
  float delta = (y_pixel_scroll_target_ - y_pixel_scroll_) * 0.2f;
  int before = y_pixel_scroll_;
  y_pixel_scroll_ += delta;
  if (before == y_pixel_scroll_) {
    y_pixel_scroll_ = y_pixel_scroll_target_;
    ticks_since_stopped_moving_++;
    return ticks_since_stopped_moving_ <
           (kFadeOutAfterTicks + kFadeOutOverTicks);
  }
  ticks_since_stopped_moving_ = 0;
  return true;
}

void ScrollHelper::RenderScrollIndicators(
    Renderer* renderer, const Skin& skin) {
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

  const Texture* top_texture = skin.vscrollbar_top_texture();
  const Texture* middle_texture = skin.vscrollbar_middle_texture();
  const Texture* bottom_texture = skin.vscrollbar_bottom_texture();
  int scrollbar_middle_height =
      scrollbar_height - top_texture->height - bottom_texture->height;
  int x = screen_rect.w - middle_texture->width;
  int y = scrollbar_offset;
  renderer->DrawTexturedRectAlpha(
      top_texture,
      Rect(x, y, top_texture->width, top_texture->height),
      alpha,
      0, 0, 1, 1);
  y += top_texture->height;
  renderer->DrawTexturedRectAlpha(
      middle_texture,
      Rect(x, y, middle_texture->width, scrollbar_middle_height),
      alpha,
      0, 0, 1, 1);
  y += scrollbar_middle_height;
  renderer->DrawTexturedRectAlpha(
      bottom_texture,
      Rect(x, y, bottom_texture->width, bottom_texture->height),
      alpha,
      0, 0, 1, 1);
}

bool ScrollHelper::ClampScrollTarget() {
  y_pixel_scroll_target_ = std::max(0, y_pixel_scroll_target_);
  int largest_possible =
      data_provider_->GetContentSize() - num_pixels_in_line_;
  y_pixel_scroll_target_ = std::min(largest_possible, y_pixel_scroll_target_);
  // Not this, if we want the scrollbar to re-appear if, e.g. you press up
  // while at the top of the document.
  // return y_pixel_scroll_ != y_pixel_scroll_target_;
  ticks_since_stopped_moving_ = 0;
  return true;
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

void ScrollHelper::CommonNotifyKey(
    InputKey key,
    bool down,
    const InputModifiers& modifiers,
    bool* invalidate,
    bool* handled) {
  *invalidate = false;
  *handled = false;
  if (key == kDown) {
    *invalidate = ScrollLines(1);
    *handled = true;
  } else if (key == kUp) {
    *invalidate = ScrollLines(-1);
    *handled = true;
  } else if (key == kPageUp || (key == kSpace && modifiers.ShiftPressed())) {
    *invalidate = ScrollPages(-1);
    *handled = true;
  } else if (key == kPageDown || (key == kSpace && !modifiers.ShiftPressed())) {
    *invalidate = ScrollPages(1);
    *handled = true;
  } else if (key == kHome) {
    *invalidate = ScrollToBeginning();
    *handled = true;
  } else if (key == kEnd) {
    *invalidate = ScrollToEnd();
    *handled = true;
  }
}

void ScrollHelper::CommonMouseWheel(
    int delta,
    const InputModifiers& modifiers,
    bool* invalidate,
    bool* handled) {
  ScrollPixels(-delta * .5f);
  *invalidate = true;
  *handled = true;
}
