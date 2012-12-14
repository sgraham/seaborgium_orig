// Copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef SG_RENDER_APPLICATION_WINDOW_H_
#define SG_RENDER_APPLICATION_WINDOW_H_

#include "sg/render/gpu.h"
class Workspace;
class DebugPresenterNotify;

class ApplicationWindow {
 public:
  ApplicationWindow() {}
  virtual ~ApplicationWindow() {}

  virtual void Show() = 0;
  virtual void Paint() = 0;
  // Corresponds to screen that this window is on, not necessarily the window
  // size itself. TODO(config): This probably should be removed because it
  // should be user-configured anyway.
  virtual bool IsLandscape() = 0;

  virtual void SetContents(Workspace* contents) = 0;
  virtual Workspace* GetContents() = 0;

  virtual void SetDebugPresenterNotify(DebugPresenterNotify* notifier) = 0;

  static ApplicationWindow* Create();
};

#endif  // SG_RENDER_APPLICATION_WINDOW_H_
