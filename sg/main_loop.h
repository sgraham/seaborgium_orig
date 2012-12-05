// Copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef SG_MAIN_LOOP_H_
#define SG_MAIN_LOOP_H_

#include "base/basictypes.h"
#include "base/memory/scoped_ptr.h"
#include "base/message_loop.h"

class AppThread;

class MainLoop {
 public:
  MainLoop();
  ~MainLoop();

  void Init();
  void MainMessageLoopStart();
  void CreateThreads();
  void MainMessageLoopRun();
  void ShutdownThreadsAndCleanUp();
  int GetResultCode();

 private:
  scoped_ptr<MessageLoop> main_message_loop_;
  scoped_ptr<AppThread> main_thread_;

  int result_code_;

  // Members initialized in |RunMainMessageLoopParts()|
  scoped_ptr<AppThread> file_thread_;
  scoped_ptr<AppThread> backend_thread_;
  scoped_ptr<AppThread> aux_thread_;

  DISALLOW_COPY_AND_ASSIGN(MainLoop);
};

#endif  // SG_MAIN_LOOP_H_
