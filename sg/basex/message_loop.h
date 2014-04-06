// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef SG_BASE_MESSAGE_LOOP_H_
#define SG_BASE_MESSAGE_LOOP_H_

#include <functional>
#include <memory>
#include <queue>

namespace base {

typedef std::function<void(void)> TaskX;
typedef std::queue<TaskX> TaskQueueX;

// XXX Rename after cr base is removed
class MessagePumpX {
 public:
  class Delegate {
   public:
    virtual ~Delegate();

    virtual bool DoWork() = 0;

    virtual bool DoIdleWork() = 0;
  };
  MessagePumpX();
  virtual ~MessagePumpX();

  virtual void Run(Delegate* delegate) = 0;

  // Quit the run loop. Can only be called on the thread that called Run.
  virtual void Quit() = 0;
};

class MessagePumpSdl : public MessagePumpX {
 public:
  MessagePumpSdl();
  virtual ~MessagePumpSdl();

  virtual void Run(Delegate* delegate) override;
  virtual void Quit() override;

 private:
  bool HandleSystemEvent();

  bool should_quit_;
};

class MessageLoopX : public MessagePumpX::Delegate {
 public:
  explicit MessageLoopX(std::unique_ptr<base::MessagePumpX> pump);
  virtual ~MessageLoopX();

  void PostTask(TaskX& task);

  void RunIt();  // XXX Rename after cr base is removed.

  // Implementation of MessagePumpX::Delegate:
  virtual bool DoWork();
  virtual bool DoIdleWork();

 private:
  std::unique_ptr<MessagePumpX> pump_;
  TaskQueueX work_queue_;
};

}  // namespace base

#endif  // SG_BASE_MESSAGE_LOOP_H_
