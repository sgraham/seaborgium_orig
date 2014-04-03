// Copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef SG_BACKEND_PROCESS_NATIVE_WIN_H_
#define SG_BACKEND_PROCESS_NATIVE_WIN_H_

#include <memory>
#include <vector>

#include "sg/backend/backend.h"

class DebugConnectionNativeWin;
namespace base {
class Thread;
}

// TODO(scottmg): All temp.
class ProcessNativeWin : public Process {
 public:
  ProcessNativeWin();
  virtual ~ProcessNativeWin();
  PROCESS_INFORMATION process_information;

  void StartEventLoop(DebugConnectionNativeWin* main);

  // (Non-)Implementation of Process.
  virtual bool Suspend(string16* error) { return false; }
  virtual bool Resume(string16* error) { return false; }
  virtual bool GetName(string16* name, string16* error) { return false; }
  virtual void GetThreadList(std::vector<ThreadId>* result) {}
  virtual Thread* GetThread(ThreadId thread_id, string16* error) {
    return NULL;
  }
  virtual void GetHeapList(std::vector<HeapId>* result) {}
  virtual Heap* GetHeap(HeapId heap_id, string16* error) { return NULL; }

  void DebugEventLoop();

 private:
  std::unique_ptr<base::Thread> event_loop_thread_;
};

#endif  // SG_BACKEND_PROCESS_NATIVE_WIN_H_
