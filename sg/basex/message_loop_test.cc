// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "sg/basex/message_loop.h"

#include <gtest/gtest.h>

using namespace base;

TEST(MessageLoopTest, Setup) {
  MessageLoopX loop(std::unique_ptr<MessagePumpX>(new MessagePumpSdl));
  loop.RunIt();
}
