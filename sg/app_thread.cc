// Copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "sg/app_thread.h"

#include <string>

#include "base/bind.h"
#include "base/compiler_specific.h"
#include "base/lazy_instance.h"
#include "base/message_loop.h"
#include "base/message_loop_proxy.h"
#include "base/threading/thread_restrictions.h"

namespace {

// Friendly names for the well-known threads.
static const char* g_app_thread_names[AppThread::ID_COUNT] = {
  "",  // UI (name assembled in main_loop.cc).
  "Seaborgium_FileThread",  // FILE
  "Seaborgium_BackendThread",  // BACKEND
  "Seaborgium_GpuThread",  // GPU
};

struct AppThreadGlobals {
  AppThreadGlobals() {
    memset(threads, 0, AppThread::ID_COUNT * sizeof(threads[0]));
  }

  // This lock protects |threads|. Do not read or modify that array
  // without holding this lock. Do not block while holding this lock.
  base::Lock lock;

  // This array is protected by |lock|. The threads are not owned by this
  // array. Typically, the threads are owned on the UI thread by MainLoop.
  // AppThread objects remove themselves from this array upon destruction.
  AppThread* threads[AppThread::ID_COUNT];
};

base::LazyInstance<AppThreadGlobals>::Leaky
    g_globals = LAZY_INSTANCE_INITIALIZER;

}  // namespace

AppThread::AppThread(ID identifier)
    : Thread(g_app_thread_names[identifier]),
      identifier_(identifier) {
  Initialize();
}

AppThread::AppThread(ID identifier,
                                     MessageLoop* message_loop)
    : Thread(message_loop->thread_name().c_str()),
      identifier_(identifier) {
  set_message_loop(message_loop);
  Initialize();
}

void AppThread::Initialize() {
  AppThreadGlobals& globals = g_globals.Get();

  base::AutoLock lock(globals.lock);
  DCHECK(identifier_ >= 0 && identifier_ < ID_COUNT);
  DCHECK(globals.threads[identifier_] == NULL);
  globals.threads[identifier_] = this;
}

AppThread::~AppThread() {
  // All Thread subclasses must call Stop() in the destructor. This is
  // doubly important here as various bits of code check they are on
  // the right AppThread.
  Stop();

  AppThreadGlobals& globals = g_globals.Get();
  base::AutoLock lock(globals.lock);
  globals.threads[identifier_] = NULL;
#ifndef NDEBUG
  // Double check that the threads are ordered correctly in the enumeration.
  for (int i = identifier_ + 1; i < ID_COUNT; ++i) {
    DCHECK(!globals.threads[i]) <<
        "Threads must be listed in the reverse order that they die";
  }
#endif
}

// We disable optimizations for this block of functions so the compiler doesn't
// merge them all together.
MSVC_DISABLE_OPTIMIZE()
MSVC_PUSH_DISABLE_WARNING(4748)

NOINLINE void AppThread::UIThreadRun(MessageLoop* message_loop) {
  volatile int line_number = __LINE__;
  Thread::Run(message_loop);
  CHECK_GT(line_number, 0);
}

NOINLINE void AppThread::FileThreadRun(MessageLoop* message_loop) {
  volatile int line_number = __LINE__;
  Thread::Run(message_loop);
  CHECK_GT(line_number, 0);
}

NOINLINE void AppThread::BackendThreadRun(MessageLoop* message_loop) {
  volatile int line_number = __LINE__;
  Thread::Run(message_loop);
  CHECK_GT(line_number, 0);
}

NOINLINE void AppThread::GpuThreadRun(MessageLoop* message_loop) {
  volatile int line_number = __LINE__;
  Thread::Run(message_loop);
  CHECK_GT(line_number, 0);
}

MSVC_POP_WARNING()
MSVC_ENABLE_OPTIMIZE();

void AppThread::Run(MessageLoop* message_loop) {
  AppThread::ID thread_id;
  if (!GetCurrentThreadIdentifier(&thread_id))
    return Thread::Run(message_loop);

  switch (thread_id) {
    case AppThread::UI:
      return UIThreadRun(message_loop);
    case AppThread::FILE:
      return FileThreadRun(message_loop);
    case AppThread::BACKEND:
      return BackendThreadRun(message_loop);
    case AppThread::GPU:
      return GpuThreadRun(message_loop);
    case AppThread::ID_COUNT:
      CHECK(false);  // This shouldn't actually be reached!
      break;
  }
  Thread::Run(message_loop);
}

// static
bool AppThread::PostTaskHelper(
    AppThread::ID identifier,
    const tracked_objects::Location& from_here,
    const base::Closure& task,
    base::TimeDelta delay,
    bool nestable) {
  DCHECK(identifier >= 0 && identifier < ID_COUNT);
  // Optimization: to avoid unnecessary locks, we listed the ID enumeration in
  // order of lifetime.  So no need to lock if we know that the target thread
  // outlives current thread.
  // Note: since the array is so small, ok to loop instead of creating a map,
  // which would require a lock because std::map isn't thread safe, defeating
  // the whole purpose of this optimization.
  AppThread::ID current_thread;
  bool target_thread_outlives_current =
      GetCurrentThreadIdentifier(&current_thread) &&
      current_thread >= identifier;

  AppThreadGlobals& globals = g_globals.Get();
  if (!target_thread_outlives_current)
    globals.lock.Acquire();

  MessageLoop* message_loop = globals.threads[identifier] ?
      globals.threads[identifier]->message_loop() : NULL;
  if (message_loop) {
    if (nestable) {
      message_loop->PostDelayedTask(from_here, task, delay);
    } else {
      message_loop->PostNonNestableDelayedTask(from_here, task, delay);
    }
  }

  if (!target_thread_outlives_current)
    globals.lock.Release();

  return !!message_loop;
}

// An implementation of MessageLoopProxy to be used in conjunction
// with AppThread.
class AppThreadMessageLoopProxy : public base::MessageLoopProxy {
 public:
  explicit AppThreadMessageLoopProxy(AppThread::ID identifier)
      : id_(identifier) {
  }

  // MessageLoopProxy implementation.
  virtual bool PostDelayedTask(
      const tracked_objects::Location& from_here,
      const base::Closure& task, base::TimeDelta delay) OVERRIDE {
    return AppThread::PostDelayedTask(id_, from_here, task, delay);
  }

  virtual bool PostNonNestableDelayedTask(
      const tracked_objects::Location& from_here,
      const base::Closure& task,
      base::TimeDelta delay) OVERRIDE {
    return AppThread::PostNonNestableDelayedTask(id_, from_here, task,
                                                     delay);
  }

  virtual bool RunsTasksOnCurrentThread() const OVERRIDE {
    return AppThread::CurrentlyOn(id_);
  }

 protected:
  virtual ~AppThreadMessageLoopProxy() {}

 private:
  AppThread::ID id_;
  DISALLOW_COPY_AND_ASSIGN(AppThreadMessageLoopProxy);
};

// static
bool AppThread::CurrentlyOn(ID identifier) {
  // We shouldn't use MessageLoop::current() since it uses LazyInstance which
  // may be deleted by ~AtExitManager when a WorkerPool thread calls this
  // function.
  // http://crbug.com/63678
  base::ThreadRestrictions::ScopedAllowSingleton allow_singleton;
  AppThreadGlobals& globals = g_globals.Get();
  base::AutoLock lock(globals.lock);
  DCHECK(identifier >= 0 && identifier < ID_COUNT);
  return globals.threads[identifier] &&
         globals.threads[identifier]->message_loop() ==
             MessageLoop::current();
}

// static
bool AppThread::IsMessageLoopValid(ID identifier) {
  if (g_globals == NULL)
    return false;

  AppThreadGlobals& globals = g_globals.Get();
  base::AutoLock lock(globals.lock);
  DCHECK(identifier >= 0 && identifier < ID_COUNT);
  return globals.threads[identifier] &&
         globals.threads[identifier]->message_loop();
}

// static
bool AppThread::PostTask(ID identifier,
                             const tracked_objects::Location& from_here,
                             const base::Closure& task) {
  return AppThread::PostTaskHelper(
      identifier, from_here, task, base::TimeDelta(), true);
}

// static
bool AppThread::PostDelayedTask(ID identifier,
                                    const tracked_objects::Location& from_here,
                                    const base::Closure& task,
                                    base::TimeDelta delay) {
  return AppThread::PostTaskHelper(
      identifier, from_here, task, delay, true);
}

// static
bool AppThread::PostNonNestableTask(
    ID identifier,
    const tracked_objects::Location& from_here,
    const base::Closure& task) {
  return AppThread::PostTaskHelper(
      identifier, from_here, task, base::TimeDelta(), false);
}

// static
bool AppThread::PostNonNestableDelayedTask(
    ID identifier,
    const tracked_objects::Location& from_here,
    const base::Closure& task,
    base::TimeDelta delay) {
  return AppThread::PostTaskHelper(
      identifier, from_here, task, delay, false);
}

// static
bool AppThread::PostTaskAndReply(
    ID identifier,
    const tracked_objects::Location& from_here,
    const base::Closure& task,
    const base::Closure& reply) {
  return GetMessageLoopProxyForThread(identifier)->PostTaskAndReply(from_here,
                                                                    task,
                                                                    reply);
}

// static
bool AppThread::GetCurrentThreadIdentifier(ID* identifier) {
  if (g_globals == NULL)
    return false;

  // We shouldn't use MessageLoop::current() since it uses LazyInstance which
  // may be deleted by ~AtExitManager when a WorkerPool thread calls this
  // function.
  // http://crbug.com/63678
  base::ThreadRestrictions::ScopedAllowSingleton allow_singleton;
  MessageLoop* cur_message_loop = MessageLoop::current();
  AppThreadGlobals& globals = g_globals.Get();
  for (int i = 0; i < ID_COUNT; ++i) {
    if (globals.threads[i] &&
        globals.threads[i]->message_loop() == cur_message_loop) {
      *identifier = globals.threads[i]->identifier_;
      return true;
    }
  }

  return false;
}

// static
scoped_refptr<base::MessageLoopProxy>
AppThread::GetMessageLoopProxyForThread(ID identifier) {
  scoped_refptr<base::MessageLoopProxy> proxy(
      new AppThreadMessageLoopProxy(identifier));
  return proxy;
}
