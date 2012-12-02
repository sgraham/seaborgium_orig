// Copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef SG_APP_THREAD_H_
#define SG_APP_THREAD_H_

#include "base/basictypes.h"
#include "base/location.h"
#include "base/message_loop.h"
#include "base/threading/thread.h"

class AppThread : public base::Thread {
 public:
  enum ID {
    // The main thread.
    UI,

    // This is the thread that interacts with the file system.
    FILE,

    // This thread is for the main operations of the debugging backend.
    BACKEND,

    // This thread is for rendering/gpu operations.
    GPU,

    // This identifier does not represent a thread.  Instead it counts the
    // number of well-known threads.
    ID_COUNT
  };

  // Construct a BrowserThreadImpl with the supplied identifier.  It is an error
  // to construct a BrowserThreadImpl that already exists.
  explicit AppThread(AppThread::ID identifier);

  // Special constructor for the main (UI) thread.
  AppThread(AppThread::ID identifier, MessageLoop* message_loop);

  virtual ~AppThread();

  // These are the same methods in message_loop.h, but are guaranteed to either
  // get posted to the MessageLoop if it's still alive, or be deleted otherwise.
  // They return true iff the thread existed and the task was posted.  Note that
  // even if the task is posted, there's no guarantee that it will run, since
  // the target thread may already have a Quit message in its queue.
  static bool PostTask(ID identifier,
                       const tracked_objects::Location& from_here,
                       const base::Closure& task);
  static bool PostDelayedTask(ID identifier,
                              const tracked_objects::Location& from_here,
                              const base::Closure& task,
                              base::TimeDelta delay);
  static bool PostNonNestableTask(ID identifier,
                                  const tracked_objects::Location& from_here,
                                  const base::Closure& task);
  static bool PostNonNestableDelayedTask(
      ID identifier,
      const tracked_objects::Location& from_here,
      const base::Closure& task,
      base::TimeDelta delay);

  static bool PostTaskAndReply(
      ID identifier,
      const tracked_objects::Location& from_here,
      const base::Closure& task,
      const base::Closure& reply);

  template <typename ReturnType>
  static bool PostTaskAndReplyWithResult(
      ID identifier,
      const tracked_objects::Location& from_here,
      const base::Callback<ReturnType(void)>& task,
      const base::Callback<void(ReturnType)>& reply) {
    scoped_refptr<base::MessageLoopProxy> message_loop_proxy =
        GetMessageLoopProxyForThread(identifier);
    return base::PostTaskAndReplyWithResult<ReturnType>(
        message_loop_proxy.get(), from_here, task, reply);
  }

  // Callable on any thread.  Returns whether you're currently on a particular
  // thread.
  static bool CurrentlyOn(ID identifier);

  // Callable on any thread.  Returns whether the threads message loop is valid.
  // If this returns false it means the thread is in the process of shutting
  // down.
  static bool IsMessageLoopValid(ID identifier);

  // If the current message loop is one of the known threads, returns true and
  // sets identifier to its ID.  Otherwise returns false.
  static bool GetCurrentThreadIdentifier(ID* identifier);

  // Callers can hold on to a refcounted MessageLoopProxy beyond the lifetime
  // of the thread.
  static scoped_refptr<base::MessageLoopProxy> GetMessageLoopProxyForThread(
      ID identifier);

 protected:
  virtual void Run(MessageLoop* message_loop) OVERRIDE;

 private:
  // The following are unique function names that makes it possible to tell
  // the thread id from the callstack alone in crash dumps.
  void UIThreadRun(MessageLoop* message_loop);
  void FileThreadRun(MessageLoop* message_loop);
  void BackendThreadRun(MessageLoop* message_loop);
  void GpuThreadRun(MessageLoop* message_loop);

  static bool PostTaskHelper(
      ID identifier,
      const tracked_objects::Location& from_here,
      const base::Closure& task,
      base::TimeDelta delay,
      bool nestable);

  // Common initialization code for the constructors.
  void Initialize();

  ID identifier_;

  DISALLOW_COPY_AND_ASSIGN(AppThread);
};

#endif  // SG_APP_THREAD_H_
