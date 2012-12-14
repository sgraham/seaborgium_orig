// Copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef SG_STATUS_BAR_H_
#define SG_STATUS_BAR_H_

#include "base/string16.h"
#include "sg/ui/contents.h"

class StatusBar : public Contents {
 public:
  explicit StatusBar(const Skin& skin);
  virtual ~StatusBar();

  void SetRenderTime(double ms_per_frame);
  void SetDebugState(const string16& status);

  virtual void Render(Renderer* renderer);

 private:
  string16 ms_per_frame_;
  string16 debug_state_;
};

#endif  // SG_STATUS_BAR_H_
