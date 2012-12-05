// Copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "sg/main_loop.h"

#include "base/logging.h"
#include "base/run_loop.h"
#include "base/threading/thread_restrictions.h"
#include "sg/app_thread.h"

namespace {
MainLoop* g_current_main_loop = NULL;
}  // namespace

MainLoop::MainLoop() : result_code_(0) {
  DCHECK(!g_current_main_loop);
  g_current_main_loop = this;
}

MainLoop::~MainLoop() {
  DCHECK(g_current_main_loop == this);
  g_current_main_loop = NULL;
}

void MainLoop::Init() {
}

void MainLoop::MainMessageLoopStart() {
  if (!MessageLoop::current())
    main_message_loop_.reset(new MessageLoop(MessageLoop::TYPE_UI));

  const char* kThreadName = "SeaborgiumMain";
  base::PlatformThread::SetName(kThreadName);
  if (main_message_loop_.get())
    main_message_loop_->set_thread_name(kThreadName);

  // Register the main thread by instantiating it, but don't call any methods.
  main_thread_.reset(new AppThread(AppThread::UI, MessageLoop::current()));
}

void MainLoop::CreateThreads() {
  if (result_code_ > 0)
    return;

  base::Thread::Options default_options;
  base::Thread::Options io_message_loop_options;
  io_message_loop_options.message_loop_type = MessageLoop::TYPE_IO;
  base::Thread::Options ui_message_loop_options;
  ui_message_loop_options.message_loop_type = MessageLoop::TYPE_UI;

  // Start threads in the order they occur in the AppThread::ID
  // enumeration, except for AppThread::UI which is the main
  // thread.
  //
  // Must be size_t so we can increment it.
  for (size_t thread_id = AppThread::UI + 1;
       thread_id < AppThread::ID_COUNT;
       ++thread_id) {
    scoped_ptr<AppThread>* thread_to_start = NULL;
    base::Thread::Options* options = &default_options;

    switch (thread_id) {
      case AppThread::FILE:
        thread_to_start = &file_thread_;
        options = &io_message_loop_options;
        break;
      case AppThread::BACKEND:
        thread_to_start = &backend_thread_;
        options = &io_message_loop_options;
        break;
      case AppThread::AUX:
        thread_to_start = &aux_thread_;
        // Maybe unnecessary? It might have to pump on Windows.
        options = &ui_message_loop_options;
        break;
      case AppThread::UI:
      case AppThread::ID_COUNT:
      default:
        NOTREACHED();
        break;
    }

    AppThread::ID id = static_cast<AppThread::ID>(thread_id);

    if (thread_to_start) {
      (*thread_to_start).reset(new AppThread(id));
      (*thread_to_start)->StartWithOptions(*options);
    } else {
      NOTREACHED();
    }
  }

  // If the UI thread blocks, the whole UI is unresponsive.
  // Do not allow disk IO from the UI thread.
  base::ThreadRestrictions::SetIOAllowed(false);
  base::ThreadRestrictions::DisallowWaiting();
}


void MainLoop::MainMessageLoopRun() {
  DCHECK_EQ(MessageLoop::TYPE_UI, MessageLoop::current()->type());
  base::RunLoop run_loop;
  run_loop.Run();
}

void MainLoop::ShutdownThreadsAndCleanUp() {
  base::ThreadRestrictions::SetIOAllowed(true);

  // Must be size_t so we can subtract from it.
  for (size_t thread_id = AppThread::ID_COUNT - 1;
       thread_id >= (AppThread::UI + 1);
       --thread_id) {
    // Find the thread object we want to stop. Looping over all valid
    // AppThread IDs and DCHECKing on a missing case in the switch
    // statement helps avoid a mismatch between this code and the
    // AppThread::ID enumeration.
    //
    // The destruction order is the reverse order of occurrence in the
    // AppThread::ID list.
    scoped_ptr<AppThread>* thread_to_stop = NULL;
    switch (thread_id) {
      case AppThread::FILE:
        thread_to_stop = &file_thread_;
        break;
      case AppThread::BACKEND:
        thread_to_stop = &backend_thread_;
        break;
      case AppThread::AUX:
        thread_to_stop = &aux_thread_;
        break;
      case AppThread::UI:
      case AppThread::ID_COUNT:
      default:
        NOTREACHED();
        break;
    }

    if (thread_to_stop) {
      thread_to_stop->reset();
    } else {
      NOTREACHED();
    }
  }
}

int MainLoop::GetResultCode() {
  return result_code_;
}
