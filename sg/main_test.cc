// Copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "gtest/gtest.h"

#include "base/at_exit.h"
#include "base/time.h"

GTEST_API_ int main(int argc, char **argv) {
  base::AtExitManager exit_manager;
  base::Time::EnableHighResolutionTimer(true);
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
