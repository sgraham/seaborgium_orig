// Copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "sg/backend/backend_native_win.h"

#include <windows.h>

#include "base/logging.h"
#include "base/memory/scoped_ptr.h"
#include "sg/debug_presenter_notify.h"

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

DebugConnectionNativeWin::DebugConnectionNativeWin(
    DebugPresenterNotify* debug_presenter_notify)
    : debug_presenter_notify_(debug_presenter_notify) {
  string16 name;
  GetName(&name, NULL);
  debug_presenter_notify_->NotifyDebugStateChanged(name);
}

DebugConnectionNativeWin::~DebugConnectionNativeWin() {
}

bool DebugConnectionNativeWin::GetName(string16* name, string16* error) {
  *name = L"Win32 (native)";
  return true;
}

Process* DebugConnectionNativeWin::ProcessCreate(
    const string16& application,
    const string16& command_line,
    const std::vector<string16> environment,
    const string16& working_directory,
    string16* error) {
  DCHECK_EQ(0, environment.size()) << "todo;";
  DCHECK_EQ(0, working_directory.size()) << "todo;";
  scoped_ptr<ProcessNativeWin> process(new ProcessNativeWin);
  // CreateProcessW can modify input buffer so we have to make a copy here.
  scoped_array<char16> command_line_copy(new char16[command_line.size() + 1]);
  command_line.copy(command_line_copy.get(), command_line.size());
  command_line_copy[command_line.size()] = 0;
  BOOL result = CreateProcess(
      application.c_str(),
      command_line_copy.get(),
      NULL, NULL,
      FALSE,
      CREATE_SUSPENDED | DEBUG_PROCESS | DETACHED_PROCESS,
      NULL,  // environment
      NULL,  // working directory
      NULL,
      &process->process_information);
  if (result)
    return process.release();
  // TODO(scottmg): Fill out error.
  return NULL;
}

void DebugConnectionNativeWin::GetProcessList(std::vector<ProcessId>* result) {
  result->clear();
}

Process* DebugConnectionNativeWin::ProcessAttach(
    ProcessId process_id, string16* error) {
  return NULL;
}
