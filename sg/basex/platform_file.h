// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef SG_BASE_PLATFORM_FILE_H_
#define SG_BASE_PLATFORM_FILE_H_

#include "sg/basex/build_config.h"

namespace base {

#if defined(OS_WIN)
typedef HANDLE PlatformFile;
#elif defined(OS_POSIX)
typedef int PlatformFile;
#endif

}  // namespace base

#endif  // SG_BASE_PLATFORM_FILE_H_
