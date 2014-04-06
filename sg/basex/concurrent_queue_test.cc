// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "sg/basex/concurrent_queue.h"

#include <gtest/gtest.h>

#include <future>

namespace {

void PushRangeAndSleepRandomly(ConcurrentQueue<int>* q, int low, int high) {
  for (int i = low; i != high; ++i) {
    q->Push(i);
    if ((rand() % 4) == 0)  // NOLINT(runtime/threadsafe_fn)
      std::this_thread::sleep_for(std::chrono::seconds(0));
  }
}

}  // namespace

TEST(ConcurrentQueueTest, Basic) {
  ConcurrentQueue<int> q;

  for (int i = 10; i < 100; ++i)
    q.Push(i);

  for (int i = 10; i < 100; ++i) {
    int j;
    q.Wait(&j);
    EXPECT_EQ(i, j);
  }
}

TEST(ConcurrentQueueTest, Racing) {
  ConcurrentQueue<int> q;

  // Push [0..999], trying to race.
  auto a = std::async(std::launch::async,
                      [&] { PushRangeAndSleepRandomly(&q, 20, 36); });
  auto b = std::async(std::launch::async,
                      [&] { PushRangeAndSleepRandomly(&q, 7, 20); });
  auto c = std::async(std::launch::async,
                      [&] { PushRangeAndSleepRandomly(&q, 36, 501); });
  auto d = std::async(std::launch::async,
                      [&] { PushRangeAndSleepRandomly(&q, 501, 1000); });
  auto e = std::async(std::launch::async,
                      [&] { PushRangeAndSleepRandomly(&q, 0, 5); });
  auto f = std::async(std::launch::async,
                      [&] { PushRangeAndSleepRandomly(&q, 5, 7); });
  a.get();
  b.get();
  c.get();
  d.get();
  e.get();
  f.get();

  std::vector<int> result;
  for (int i = 0; i < 1000; ++i) {
    int j;
    q.Wait(&j);
    result.push_back(j);
  }
  std::sort(result.begin(), result.end());
  for (int i = 0; i < 1000; ++i)
    EXPECT_EQ(result[i], i);
}
