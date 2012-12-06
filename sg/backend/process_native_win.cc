// Copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "sg/backend/process_native_win.h"

#include "base/bind.h"
#include "base/threading/thread.h"
#include "sg/backend/backend_native_win.h"
#include "sg/backend/debug_core_native_win.h"

ProcessNativeWin::ProcessNativeWin() {
  memset(&process_information, 0, sizeof(process_information));
}

ProcessNativeWin::~ProcessNativeWin() {
}

void ProcessNativeWin::StartEventLoop(
    DebugConnectionNativeWin* debug_connection) {
  // TODO(scottmg): Put process and main thread id in name of thread.
  event_loop_thread_.reset(new base::Thread("Native debugger event loop"));
  event_loop_thread_->Start();
  event_loop_thread_->message_loop()->PostTask(
      FROM_HERE, base::Bind(&ProcessNativeWin::DebugEventLoop,
                            base::Unretained(this))); // TODO(lifetime)
}

void ProcessNativeWin::DebugEventLoop() {
  DWORD continue_status = DBG_CONTINUE; // exception continuation
  DEBUG_EVENT debug_event = {0};
  for (;;) {
    // Block until the next event.
    BOOL result = WaitForDebugEvent(&debug_event, INFINITE);
    if (!result) {
      OutputDebugString(FormatLastError(L"WaitForDebugEvent").c_str());
    }

    switch (debug_event.dwDebugEventCode) {
      case EXCEPTION_DEBUG_EVENT:
        // Process the exception code. When handling
        // exceptions, remember to set the continuation
        // status parameter (continue_status). This value
        // is used by the ContinueDebugEvent function.

        switch(debug_event.u.Exception.ExceptionRecord.ExceptionCode) {
          case EXCEPTION_ACCESS_VIOLATION:
            // First chance: Pass this on to the system.
            // Last chance: Display an appropriate error.
            break;

          case EXCEPTION_BREAKPOINT:
            // First chance: Display the current
            // instruction and register values.
            break;

          case EXCEPTION_DATATYPE_MISALIGNMENT:
            // First chance: Pass this on to the system.
            // Last chance: Display an appropriate error.
            break;

          case EXCEPTION_SINGLE_STEP:
            // First chance: Update the display of the
            // current instruction and register values.
            break;

          case DBG_CONTROL_C:
            // First chance: Pass this on to the system.
            // Last chance: Display an appropriate error.
            break;

          default:
            // Handle other exceptions.
            break;
        }

        break;

      case CREATE_THREAD_DEBUG_EVENT:
        // As needed, examine or change the thread's registers with the
        // GetThreadContext and SetThreadContext functions; and suspend and
        // resume thread execution with the SuspendThread and ResumeThread
        // functions.
        // continue_status = OnCreateThreadDebugEvent(debug_event);
        continue_status = DBG_CONTINUE;
        break;

      case CREATE_PROCESS_DEBUG_EVENT:
        // As needed, examine or change the registers of the process's initial
        // thread with the GetThreadContext and SetThreadContext functions;
        // read from and write to the process's virtual memory with the
        // ReadProcessMemory and WriteProcessMemory functions; and suspend and
        // resume thread execution with the SuspendThread and ResumeThread
        // functions. Be sure to close the handle to the process image file
        // with CloseHandle.
        // continue_status = OnCreateProcessDebugEvent(debug_event);
        continue_status = DBG_CONTINUE;
        break;

      case EXIT_THREAD_DEBUG_EVENT:
        // Display the thread's exit code.
        // continue_status = OnExitThreadDebugEvent(debug_event);
        continue_status = DBG_CONTINUE;
        break;

      case EXIT_PROCESS_DEBUG_EVENT:
        // Display the process's exit code.
        // continue_status = OnExitProcessDebugEvent(debug_event);
        continue_status = DBG_CONTINUE;
        return; // TODO(backend) XXX XXX XXX XXX
        break;

      case LOAD_DLL_DEBUG_EVENT:
        // Read the debugging information included in the newly loaded DLL. Be
        // sure to close the handle to the loaded DLL with CloseHandle.
        // continue_status = OnLoadDllDebugEvent(debug_event);
        continue_status = DBG_CONTINUE;
        break;

      case UNLOAD_DLL_DEBUG_EVENT:
        // Display a message that the DLL has been unloaded.
        // continue_status = OnUnloadDllDebugEvent(debug_event);
        continue_status = DBG_CONTINUE;
        break;

      case OUTPUT_DEBUG_STRING_EVENT:
        // Display the output debugging string.
        // continue_status = OnOutputDebugStringEvent(debug_event);
        continue_status = DBG_CONTINUE;
        break;

      case RIP_EVENT:
        // continue_status = OnRipEvent(debug_event);
        continue_status = DBG_CONTINUE;
        break;
    }

    // Resume executing the thread that reported the debugging event.
    ContinueDebugEvent(debug_event.dwProcessId,
        debug_event.dwThreadId,
        continue_status);
  }
}
