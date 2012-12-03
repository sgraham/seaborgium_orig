// Copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef SG_GLOBAL_H_
#define SG_GLOBAL_H_

#ifdef _DEBUG

#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
// base::strdup fails if this is |#define|d.
#undef strdup

#endif

#endif  // SG_GLOBAL_H_
