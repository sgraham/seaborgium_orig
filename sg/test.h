// Copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/basictypes.h"

#if defined(OS_WIN) && !defined(NDEBUG)
#include <crtdbg.h>
#endif

#include <gtest/gtest.h>

class LeakCheckTest : public testing::Test {
 public:
  LeakCheckTest() {
#if defined(OS_WIN) && !defined(NDEBUG)
    _CrtMemCheckpoint(&initial_memory_state_);
#endif
  }

  virtual ~LeakCheckTest() {
#if defined(OS_WIN) && !defined(NDEBUG)
    if (!HasFailure()) {
      _CrtMemState final_memory_state;
      _CrtMemState difference;
      _CrtMemCheckpoint(&final_memory_state);
      if (_CrtMemDifference(
          &difference, &initial_memory_state_, &final_memory_state)) {
        _CrtMemDumpAllObjectsSince(&difference);
        _CrtMemDumpStatistics(&difference);
        FailTest();
      }
    }
#endif
  }

 private:
#if defined(OS_WIN) && !defined(NDEBUG)
  _CrtMemState initial_memory_state_;
#endif
  void FailTest() {
    FAIL() << "Memory leaks detected.";
  }
};
