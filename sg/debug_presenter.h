// Copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef SG_DEBUG_PRESENTER_H_
#define SG_DEBUG_PRESENTER_H_

#include <string>

#include "base/basictypes.h"
#include "base/file_path.h"

class DebugPresenterDisplay;
class SourceFiles;

class DebugPresenter {
 public:
  explicit DebugPresenter(SourceFiles* source_files);
  virtual ~DebugPresenter();

  virtual void SetDisplay(DebugPresenterDisplay* display);

 private:
  void ReadFileOnFILE(FilePath path, std::string* result);
  void FileLoadCompleted(FilePath path, std::string* result);

  DebugPresenterDisplay* display_;
  SourceFiles* source_files_;

  DISALLOW_COPY_AND_ASSIGN(DebugPresenter);
};

#endif  // SG_DEBUG_PRESENTER_H_
