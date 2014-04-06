// Copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef SG_UI_SCROLL_HELPER_H_
#define SG_UI_SCROLL_HELPER_H_

#include "base/bind.h"
#include "sg/basex/string16.h"
#include "sg/basic_geometric_types.h"
#include "sg/ui/input.h"

class Skin;
class Renderer;

class ScrollHelperDataProvider {
 public:
  virtual ~ScrollHelperDataProvider() {}

  virtual int GetContentSize() = 0;
  virtual const Rect& GetScreenRect() const = 0;
};

// Handles scroll offsets and rendering of scroll indicators.
class ScrollHelper {
 public:
  ScrollHelper(ScrollHelperDataProvider* data_provider,
               int num_pixels_in_line);
  virtual ~ScrollHelper();

  // Returns whether invalidation is required.
  bool Update();

  void RenderScrollIndicators(Renderer* renderer, const Skin& skin);
  int GetOffset() const { return y_pixel_scroll_; }

  // Returns "did move", i.e. whether invalidation is required.
  bool ScrollPixels(int delta);
  bool ScrollLines(int delta);
  bool ScrollPages(int delta);
  bool ScrollToBeginning();
  bool ScrollToEnd();

  // Optional, standard handling of keys/mouse for scrolling.
  void CommonNotifyKey(
      InputKey key,
      bool down,
      const InputModifiers& modifiers,
      bool* invalidate,
      bool* handled);
  void CommonMouseWheel(
      int delta,
      const InputModifiers& modifiers,
      bool* invalidate,
      bool* handled);

 private:
  // Returns whether invalidation is required.
  bool ClampScrollTarget();

  // TODO(scottmg): x, Point.
  int y_pixel_scroll_;
  int y_pixel_scroll_target_;
  int ticks_since_stopped_moving_;
  int num_pixels_in_line_;
  ScrollHelperDataProvider* data_provider_;
};

#endif  // SG_UI_SCROLL_HELPER_H_
