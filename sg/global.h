// Copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef SG_GLOBAL_H_
#define SG_GLOBAL_H_

#ifdef _DEBUG
#define _CRTDBG_MAP_ALLOC
#endif

#define _INTSAFE_H_INCLUDED_  // What a cluster.

#ifdef __cplusplus
#include <algorithm>
#include <functional>
#include <iosfwd>
#include <list>
#include <map>
#include <set>
#include <stack>
#include <string>
#include <utility>
#include <vector>
#endif

#ifdef _MSC_VER
#include <crtdbg.h>  // NOLINT(build/include_order)
#endif

#include <stdio.h>  // NOLINT(build/include_order)
#include <string.h>  // NOLINT(build/include_order)
#include <stdint.h>  // NOLINT(build/include_order)
#include <stdlib.h>  // NOLINT(build/include_order)
#include <stddef.h>  // NOLINT(build/include_order)
#include <assert.h>  // NOLINT(build/include_order)
#include <stdarg.h>  // NOLINT(build/include_order)
#include <time.h>  // NOLINT(build/include_order)
// base::strdup fails if this is |#define|d.
#undef strdup

#ifdef _WIN32
#include <windows.h>  // NOLINT(build/include_order)
#endif

#endif  // SG_GLOBAL_H_
