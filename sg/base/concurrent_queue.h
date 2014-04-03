// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef SG_BASE_CONCURRENT_QUEUE_H_
#define SG_BASE_CONCURRENT_QUEUE_H_

#include "sg/base/macros.h"

#include <condition_variable>
#include <mutex>
#include <queue>
#include <thread>

template <typename T>
class ConcurrentQueue {
 public:
  void Wait(T* item) {
    std::unique_lock<std::mutex> mlock(mutex_);
    while (queue_.empty())
      cond_.wait(mlock);
    *item = queue_.front();
    queue_.pop();
  }

  // TODO(scottmg): bool Pop(T* item) ?

  void Push(const T& item) {
    std::unique_lock<std::mutex> mlock(mutex_);
    queue_.push(item);
    mlock.unlock();
    cond_.notify_one();
  }

  void Push(T&& item) {
    std::unique_lock<std::mutex> mlock(mutex_);
    queue_.push(std::move(item));
    mlock.unlock();
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
