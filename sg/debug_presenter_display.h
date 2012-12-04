// Copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef SG_DEBUG_PRESENTER_DISPLAY_H_
#define SG_DEBUG_PRESENTER_DISPLAY_H_

#include <string>

#include "base/file_util.h"

// The interface that the DebugPresenter requires of its view.
class DebugPresenterDisplay {
 public:
  virtual ~DebugPresenterDisplay();

  virtual void SetFileName(const FilePath& filename) = 0;
  virtual void SetFileData(const std::string& utf8_text) = 0;
  virtual void SetDebugState(const string16& debug_state) = 0;
  virtual void SetRenderTime(double ms_per_frame) = 0;
};

#endif  // SG_DEBUG_PRESENTER_DISPLAY_H_
