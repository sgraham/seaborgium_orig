// Copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef SG_BACKEND_BACKEND_H_
#define SG_BACKEND_BACKEND_H_

#include <vector>

#include "base/string16.h"

class FrameData {
 public:
  uintptr_t address;
  string16 function;
  string16 filename;
  int line_number;
};

class StoppedAtBreakpointData {
 public:
  FrameData frame;
};

class StoppedAfterSteppingData {
 public:
  FrameData frame;
};

class RetrievedStackData {
 public:
  std::vector<FrameData> frames;
};

class RetrievedLocalsData {
 public:
};

class DebugNotification {
 public:
  virtual ~DebugNotification() {}

  virtual void OnProcessLoaded() {}
  virtual void OnStoppedAtBreakpoint(const StoppedAtBreakpointData& data) {}
  virtual void OnStoppedAfterStepping(const StoppedAfterSteppingData& data) {}
  virtual void OnRetrievedStack(const RetrievedStackData& data) {}
  virtual void OnRetrievedLocals(const RetrievedLocalsData& data) {}
};

class ThreadCallStack {
 public:
  virtual ~ThreadCallStack() {}

  virtual size_t GetNumFrames() = 0;

  /* TODO(backend) 
  virtual void SetFrame(size_t index) = 0;
  virtual void GetLocals() = 0;
  virtual void GetArguments() = 0;
  */
};

class ThreadContext {
 public:
  virtual ~ThreadContext() {}

  // Walk the callstack.
  virtual ThreadCallStack* GetCallStack(string16* error) = 0;
};

class Thread {
 public:
  virtual ~Thread() {}

  // Suspends execution of this thread. If false is returned, |error| will be
  // filled out.
  virtual bool Suspend(string16* error) = 0;

  // Resumes execution of this thread. If false is returned, |error| will be
  // filled out.
  virtual bool Resume(string16* error) = 0;

  // Get the name of this thread if any. If false is returned, |error| will be
  // filled out.
  virtual bool GetName(string16* name, string16* error) = 0;

  // Get thread context data (instruction pointer, stack pointer, etc.). The
  // thread must be in a suspended state before this function is called. If
  // the thread context cannot be retrieved, NULL will be returned with |error|
  // filled out. The returned ThreadContext is only valid while the thread
  // stays in a suspended state.
  virtual ThreadContext* GetContext(string16* error) = 0;
};

class Heap {
 public:
  virtual ~Heap() {}
};

class Process {
 public:
  virtual ~Process() {}

  // Suspends execution of all threads in this process. If false is returned,
  // |error| will be filled out.
  virtual bool Suspend(string16* error) = 0;

  // Resumes execution of all threads in this process. If false is returned,
  // |error| will be filled out.
  virtual bool Resume(string16* error) = 0;

  // Get the name of this process. If false is returned, |error| will be
  // filled out.
  virtual bool GetName(string16* name, string16* error) = 0;


  typedef int64 ThreadId;

  // Retrieves a list of all threads in this process.
  virtual void GetThreadList(std::vector<ThreadId>* result) = 0;

  // Get a thread object for the given |thread_id|, or NULL if it could not be
  // found. If NULL is returned, |error| will be filled with a user-presentable
  // error.
  virtual Thread* GetThread(ThreadId thread_id, string16* error) = 0;


  typedef int64 HeapId;

  // Retrives a list of all the heaps in this process.
  virtual void GetHeapList(std::vector<HeapId>* result) = 0;

  // Get a heap object for the given |heap_id|, or NULL if it could not be
  // retrieved. If NULL is returned, |error| will be filled with a
  // user-presentable error.
  virtual Heap* GetHeap(HeapId heap_id, string16* error) = 0;
};

class DebugConnection {
 public:
  virtual ~DebugConnection() {}

  // Get a user-presentable name for this debugging connection (to give the
  // user an idea of what it might be able to handle debugging).
  virtual bool GetName(string16* name, string16* error) = 0;

  typedef int64 ProcessId;

  // Launches |application| and attaches to it. It should be created in
  // suspended state. |command_line|, |environment|, and |working_directory|
  // should be used to set up the initial state for the process, insofar as
  // makes sense for the target platform. Return may be NULL if the process
  // could not be created. In this case,  |error| will be filled out with a
  // user-presentable error.
  virtual Process* ProcessCreate(
      const string16& application,
      const string16& command_line,
      const std::vector<string16> environment,
      const string16& working_directory,
      string16* error) = 0;

  // Retrieves a list of all processes that can potentially be attached to.
  virtual void GetProcessList(std::vector<ProcessId>* result) = 0;

  // Attach to specified process. Process is not suspended on attachment. May
  // return NULL if |process_id| was invalid or could not be attached to. In
  // this case, |error| will be filled out with a user-presentable error.
  virtual Process* ProcessAttach(ProcessId process_id, string16* error) = 0;
};

// TODO(backend): Multiple target connections if not at a higher level.

// TODO(backend): Explicit control over how stdin/stdout/stderr are mapped
// when creating a process? How are the handles handled?

// TODO(backend): Breakpoints

// TODO(backend): Select stack frame

// TODO(backend): Get locals, arguments, etc.

// TODO(backend): Expression evaluation (!)

// TODO(backend): Exception filtering and handling. Maybe at process level?

// TODO(backend): Heap enumeration.

#endif  // SG_BACKEND_BACKEND_H_
