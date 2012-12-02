// Copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef SG_APPLICATION_WINDOW_H_
#define SG_APPLICATION_WINDOW_H_

#include "sg/gpu.h"

class ApplicationWindow {
 public:
  ApplicationWindow() {}
  virtual ~ApplicationWindow() {}

  virtual void Show() = 0;

  static ApplicationWindow* Create();
};

#endif  // SG_APPLICATION_WINDOW_H_
