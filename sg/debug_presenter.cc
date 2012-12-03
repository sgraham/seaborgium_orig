// Copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "sg/debug_presenter.h"

#include "base/bind.h"
#include "base/file_util.h"
#include "sg/app_thread.h"
#include "sg/debug_presenter_display.h"
#include "sg/source_files.h"

DebugPresenter::DebugPresenter(SourceFiles* source_files) 
    : source_files_(source_files) {
  std::string* result = new std::string;
  FilePath path(FILE_PATH_LITERAL("sample_source_code_file.cc"));
  // Lifetime OK (I think) because DebugPresenter's lifetime outlives the FILE
  // thread.
  AppThread::PostTaskAndReply(AppThread::FILE, FROM_HERE,
      base::Bind(&DebugPresenter::ReadFileOnFILE,
                 base::Unretained(this), path, result),
      base::Bind(&DebugPresenter::FileLoadCompleted,
                 base::Unretained(this), path, result));
}

DebugPresenter::~DebugPresenter() {
}

void DebugPresenter::SetDisplay(DebugPresenterDisplay* display) {
  display_ = display;
}

void DebugPresenter::ReadFileOnFILE(FilePath path, std::string* result) {
  file_util::ReadFileToString(path, result);
}

void DebugPresenter::FileLoadCompleted(
    FilePath path, std::string* result) {
  // TODO(scottmg): mtime.
  source_files_->SetFileData(path, 0, *result);
  display_->SetFileName(path);
  display_->SetFileData(*result);
  delete result;
}
