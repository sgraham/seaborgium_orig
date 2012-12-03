// Copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef SG_GPU_H_
#define SG_GPU_H_

#include "base/basictypes.h"
#include "sg/ui/base_types.h"

// TODO(rendering): Maybe move this to a gpu_win.h.
#if defined(OS_WIN)
#include <windows.h>  // NOLINT(build/include_order)
typedef HWND RenderingSurface;
#endif

class ApplicationWindow;

class Gpu {
 public:
  static void Paint(ApplicationWindow* window);
  static void InitializeForRenderingSurface(
      ApplicationWindow* window,
      RenderingSurface surface);
  static void Resize(ApplicationWindow* window, Rect rect);

 private:
  static void OneTimeInitialization();
};

#endif  // SG_GPU_H_
