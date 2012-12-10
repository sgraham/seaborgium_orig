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

#ifndef SG_BACKEND_SUBPROCESS_H_
#define SG_BACKEND_SUBPROCESS_H_

#ifdef _WIN32
#include <windows.h>
#else
#include <signal.h>
#endif

#include <vector>
#include <queue>

#include "base/platform_file.h"
#include "base/string16.h"
#include "sg/backend/exit_status.h"

// Subprocess wraps a single async subprocess.  It is entirely
// passive: it expects the caller to notify it when its fds are ready
// for reading, as well as call Finish() to reap the child once Done()
// is true.
class Subprocess {
 public:
  Subprocess();
  ~Subprocess();

  // Returns ExitSuccess on successful process exit, kExitInterrupted if
  // the process was interrupted, ExitFailure if it otherwise failed.
  ExitStatus Finish();

  bool Done() const;

  const string16& GetOutput() const;

  base::PlatformFile GetOutputPipe() const { return output_pipe_; }
  base::PlatformFile GetInputPipe() const { return input_pipe_; }

  bool Start(const string16& application, const string16& command_line);
  void OnPipeReady();

 private:
  string16 buf_;

  void Init();

#ifdef _WIN32

  HANDLE child_;
  HANDLE input_pipe_;
  HANDLE output_pipe_;
  HANDLE child_input_pipe_;
  HANDLE child_output_pipe_;
  OVERLAPPED overlapped_;
  char overlapped_buf_[4 << 10];
  bool is_reading_;
#else
  int fd_;
  pid_t pid_;
#endif

  friend struct SubprocessSet;
};

#endif  // SG_BACKEND_SUBPROCESS_H_
