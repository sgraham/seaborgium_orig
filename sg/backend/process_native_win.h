// Copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef SG_BACKEND_PROCESS_NATIVE_WIN_H_
#define SG_BACKEND_PROCESS_NATIVE_WIN_H_

#include "sg/backend/backend.h"

// TODO(scottmg): All temp.
class ProcessNativeWin : public Process {
 public:
  ProcessNativeWin() {
    memset(&process_information, 0, sizeof(process_information));
  }
  PROCESS_INFORMATION process_information;

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
};

#endif  // SG_BACKEND_PROCESS_NATIVE_WIN_H_
