// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "sg/basex/message_loop.h"

#include <SDL.h>

namespace base {

MessagePumpX::MessagePumpX() {
}

MessagePumpX::~MessagePumpX() {
}

MessagePumpX::Delegate::~Delegate() {
}

// This should move to message_loop_sdl.cc eventually.

MessagePumpSdl::MessagePumpSdl() {
}

MessagePumpSdl::~MessagePumpSdl() {
}

bool MessagePumpSdl::HandleSystemEvent() {
  bool did_work = false;
  SDL_Event event;
  while (SDL_PollEvent(&event)) {
    did_work = true;
    // TODO: Dispatch somewhere.
  }
  return did_work;
}

void MessagePumpSdl::Run(Delegate* delegate) {
  for (;;) {
    bool did_work = HandleSystemEvent();
    if (should_quit_)
      break;
    did_work = delegate->DoWork();
    if (did_work)
      continue;
    did_work = delegate->DoIdleWork();
    if (should_quit_)
      break;

    // We could also use MsgWaitForMultipleObjectsEx with an Event set by
    // ScheduleWork, but don't bother for now.
    SDL_Delay(0);
  }
}

void MessagePumpSdl::Quit() {
  should_quit_ = true;
}

MessageLoopX::MessageLoopX(std::unique_ptr<base::MessagePumpX> pump)
    : pump_(std::move(pump)) {}

MessageLoopX::~MessageLoopX() {
}

void MessageLoopX::PostTask(TaskX& task) {
}

void MessageLoopX::RunIt() {
  pump_->Run(this);
}

bool MessageLoopX::DoWork() {
  return false;
}

bool MessageLoopX::DoIdleWork() {
  return false;
}

}  // namespace base
