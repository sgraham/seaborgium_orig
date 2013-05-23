// Copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "sg/source_files.h"

SourceFiles::SourceFiles() {
}

SourceFiles::~SourceFiles() {
}

void SourceFiles::SetFileData(
    const base::FilePath& filename, int mtime, const std::string& contents) {
  SourceFileData data;
  data.filename = filename;
  data.mtime = mtime;
  data.contents = contents;
  files_[filename] = data;
}
