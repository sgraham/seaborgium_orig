// Copyright 2012 Google Inc. All Rights Reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

// Initially copied from ninja.

#include "sg/backend/subprocess.h"

#include <stdio.h>
#include <algorithm>

#include "base/logging.h"
#include "base/memory/scoped_ptr.h"
#include "base/stringprintf.h"
#include "base/utf_string_conversions.h"

Subprocess::Subprocess() : child_(NULL) , overlapped_(), is_reading_(false) {
  Init();
}

Subprocess::~Subprocess() {
  if (input_pipe_)
    CHECK(CloseHandle(input_pipe_)) << "CloseHandle input";
  if (output_pipe_)
  CHECK(CloseHandle(output_pipe_)) << "CloseHandle output";
  // Reap child if forgotten.
  if (child_)
    Finish();
}

void Subprocess::Init() {
  string16 pipe_name = base::StringPrintf(
      L"\\\\.\\pipe\\seaborgium_pid%lu_sp%p", GetCurrentProcessId(), this);
  string16 pipe_name_input = pipe_name + L"_in";
  string16 pipe_name_output = pipe_name + L"_out";

  input_pipe_ = ::CreateNamedPipe(pipe_name_input.c_str(),
                                  PIPE_ACCESS_INBOUND | FILE_FLAG_OVERLAPPED,
                                  PIPE_TYPE_BYTE,
                                  PIPE_UNLIMITED_INSTANCES,
                                  0, 0, INFINITE, NULL);
  CHECK(input_pipe_ != INVALID_HANDLE_VALUE) << "CreateNamedPipe, input";
  output_pipe_ = ::CreateNamedPipe(pipe_name_output.c_str(),
                                   PIPE_ACCESS_OUTBOUND | FILE_FLAG_OVERLAPPED,
                                   PIPE_TYPE_BYTE,
                                   PIPE_UNLIMITED_INSTANCES,
                                   0, 0, INFINITE, NULL);
  CHECK(output_pipe_ != INVALID_HANDLE_VALUE) << "CreateNamedPipe, output";

  // Get the opposite ends of the pipes as handles inheritable to the child
  // process.
  SECURITY_ATTRIBUTES security_attributes = {0};
  security_attributes.nLength = sizeof(SECURITY_ATTRIBUTES);
  security_attributes.bInheritHandle = TRUE;
  child_output_pipe_ = CreateFile(
      pipe_name_input.c_str(), GENERIC_WRITE, 0,
      &security_attributes, OPEN_EXISTING, 0, NULL);
  CHECK(child_output_pipe_ != INVALID_HANDLE_VALUE) <<
      "CreateFile, child_output";
  child_input_pipe_ = CreateFile(
      pipe_name_output.c_str(), GENERIC_READ, 0,
      &security_attributes, OPEN_EXISTING, 0, NULL);
  CHECK(child_input_pipe_ != INVALID_HANDLE_VALUE) <<
      "CreateFile, child_input";
}

bool Subprocess::Start(
    const string16& application,
    const string16& command_line) {
  CHECK(input_pipe_ && output_pipe_ && child_input_pipe_ && child_output_pipe_);
  STARTUPINFO startup_info = {0};
  startup_info.cb = sizeof(STARTUPINFO);
  startup_info.dwFlags = STARTF_USESTDHANDLES;
  startup_info.hStdInput = child_input_pipe_;
  startup_info.hStdOutput = child_output_pipe_;
  startup_info.hStdError = child_output_pipe_;

  PROCESS_INFORMATION process_info = {0};

  // CreateProcessW can modify input buffer so we have to make a copy here.
  string16 combined = application + L" " + command_line;
  scoped_array<char16> command_copy(new char16[combined.size() + 1]);
  combined.copy(command_copy.get(), combined.size());
  command_copy[combined.size()] = 0;

  if (!CreateProcess(application.c_str(), command_copy.get(), NULL, NULL,
                     /* inherit handles */ TRUE,
                     CREATE_NEW_PROCESS_GROUP | DETACHED_PROCESS,
                     NULL, NULL,
                     &startup_info, &process_info)) {
    CloseHandle(child_output_pipe_);
    CloseHandle(child_input_pipe_);
    return false;
  }

  // Close pipe channel only used by the child.
  CloseHandle(child_output_pipe_);
  child_output_pipe_ = NULL;
  CloseHandle(child_input_pipe_);
  child_input_pipe_ = NULL;
  CloseHandle(process_info.hThread);
  child_ = process_info.hProcess;
  return true;
}

void Subprocess::OnPipeReady() {
  NOTREACHED();
  /*
  DWORD bytes;
  if (!GetOverlappedResult(pipe_, &overlapped_, &bytes, TRUE)) {
    if (GetLastError() == ERROR_BROKEN_PIPE) {
      CloseHandle(pipe_);
      pipe_ = NULL;
      return;
    }
    NOTREACHED() << ("GetOverlappedResult");
  }

  // TODO(widechar): Is this ASCII? or ANSI? or UTF8?
  if (is_reading_ && bytes)
    buf_.append(ASCIIToUTF16(base::StringPiece(overlapped_buf_, bytes)));

  memset(&overlapped_, 0, sizeof(overlapped_));
  is_reading_ = true;
  if (!::ReadFile(pipe_, overlapped_buf_, sizeof(overlapped_buf_),
                  &bytes, &overlapped_)) {
    if (GetLastError() == ERROR_BROKEN_PIPE) {
      CloseHandle(pipe_);
      pipe_ = NULL;
      return;
    }
    if (GetLastError() != ERROR_IO_PENDING)
      NOTREACHED() << ("ReadFile");
  }
  */

  // Even if we read any bytes in the readfile call, we'll enter this
  // function again later and get them at that point.
}

ExitStatus Subprocess::Finish() {
  if (!child_)
    return kExitFailure;

  // TODO(scottmg): add error handling for all of these.
  WaitForSingleObject(child_, INFINITE);

  DWORD exit_code = 0;
  GetExitCodeProcess(child_, &exit_code);

  CloseHandle(child_);
  child_ = NULL;

  return exit_code == 0              ? kExitSuccess :
         exit_code == CONTROL_C_EXIT ? kExitInterrupted :
                                       kExitFailure;
}

bool Subprocess::Done() const {
  return input_pipe_ == NULL && output_pipe_ == NULL;
}

const string16& Subprocess::GetOutput() const {
  return buf_;
}
