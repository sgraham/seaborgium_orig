// Copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef SG_STATUS_BAR_H_
#define SG_STATUS_BAR_H_

#include "sg/basex/string16.h"

class Renderer;
class Skin;

class StatusBar {
 public:
  explicit StatusBar(const Skin& skin);
  virtual ~StatusBar();

  void SetRenderTime(double ms_per_frame);

  virtual void Render(Renderer* renderer);

 private:
  string16 ms_per_frame_;
  string16 debug_state_;
  const Skin& skin_;
};

#endif  // SG_STATUS_BAR_H_
