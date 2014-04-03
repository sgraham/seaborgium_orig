// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef SG_BASE_CONCURRENT_QUEUE_H_
#define SG_BASE_CONCURRENT_QUEUE_H_

#include <condition_variable>
#include <mutex>
#include <queue>

#include "sg/base/macros.h"

template <typename T>
class ConcurrentQueue {
 public:
  void Wait(T* item) {
    std::unique_lock<std::mutex> mlock(mutex_);
    cond_.wait(mlock, [&]() { return !queue_.empty(); });
    *item = queue_.front();
    queue_.pop();
  }

  // TODO(scottmg): bool Pop(T* item) ?

  void Push(const T& item) {
    mutex_.lock();
    queue_.push(item);
    mutex_.unlock();
    cond_.notify_one();
  }

  void Push(T&& item) {
    mutex_.lock();
    queue_.push(std::move(item));
    mutex_.unlock();
    cond_.notify_one();
  }

  ConcurrentQueue() {
  }

 private:
  std::queue<T> queue_;
  std::mutex mutex_;
  std::condition_variable cond_;

  DISALLOW_COPY_AND_ASSIGN(ConcurrentQueue<T>);
};

#endif  // SG_BASE_CONCURRENT_QUEUE_H_
