// Copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef SG_DEBUG_PRESENTER_DISPLAY_H_
#define SG_DEBUG_PRESENTER_DISPLAY_H_

#include <string>
#include <vector>

#include "base/file_util.h"

class FrameData;
class TypeNameValue;

// The interface that the DebugPresenter requires of its view.
class DebugPresenterDisplay {
 public:
  virtual ~DebugPresenterDisplay();

  virtual void SetFileName(const FilePath& filename) = 0;
  virtual void SetFileData(const std::string& utf8_text) = 0;
  virtual void SetProgramCounterLine(int line_number) = 0;
  virtual void SetStackData(const std::vector<FrameData>& frame_data,
                            int active) = 0;
  virtual void SetLocalsData(const std::vector<TypeNameValue>& locals_data) = 0;
  virtual void SetDebugState(const string16& debug_state) = 0;
  virtual void SetRenderTime(double ms_per_frame) = 0;
  virtual void AddOutput(const string16& text) = 0;
  virtual void AddLog(const string16& text) = 0;
};

#endif  // SG_DEBUG_PRESENTER_DISPLAY_H_
