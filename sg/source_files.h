// Copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef SG_SOURCE_FILES_H_
#define SG_SOURCE_FILES_H_

#include <map>
#include <string>

#include "base/basictypes.h"
#include "base/files/file_path.h"

// Model for source code.
class SourceFiles {
 public:
  SourceFiles();
  ~SourceFiles();

  void SetFileData(
      const base::FilePath& filename, int mtime, const std::string& contents);

 private:
  struct SourceFileData {
    base::FilePath filename;
    int mtime;
    std::string contents;
  };
  std::map<base::FilePath, SourceFileData> files_;

  DISALLOW_COPY_AND_ASSIGN(SourceFiles);
};

#endif  // SG_SOURCE_FILES_H_
