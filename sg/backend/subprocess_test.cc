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

// Copied from ninja.

#include "sg/backend/subprocess.h"

#include <gtest/gtest.h>

#ifndef _WIN32
// SetWithLots need setrlimit.
#include <sys/time.h>
#include <sys/resource.h>
#include <unistd.h>
#endif

namespace {

#ifdef _WIN32
const wchar_t* kSimpleCommand = L"cmd /c dir \\";
#else
const char* kSimpleCommand = "ls /";
#endif

struct SubprocessTest : public testing::Test {
  SubprocessSet subprocs_;
};

}  // anonymous namespace

// Run a command that fails and emits to stderr.
TEST_F(SubprocessTest, BadCommandStderr) {
  Subprocess* subproc = subprocs_.Add(L"cmd /c ninja_no_such_command");
  ASSERT_NE(static_cast<Subprocess*>(NULL), subproc);

  while (!subproc->Done()) {
    // Pretend we discovered that stderr was ready for writing.
    subprocs_.DoWork();
  }

  EXPECT_EQ(ExitFailure, subproc->Finish());
  EXPECT_NE(L"", subproc->GetOutput());
}

// Run a command that does not exist
TEST_F(SubprocessTest, NoSuchCommand) {
  Subprocess* subproc = subprocs_.Add(L"ninja_no_such_command");
  ASSERT_NE(static_cast<Subprocess*>(NULL), subproc);

  while (!subproc->Done()) {
    // Pretend we discovered that stderr was ready for writing.
    subprocs_.DoWork();
  }

  EXPECT_EQ(ExitFailure, subproc->Finish());
  EXPECT_NE(L"", subproc->GetOutput());
#ifdef _WIN32
  ASSERT_EQ(L"CreateProcess failed: " \
      L"The system cannot find the file specified.\n",
      subproc->GetOutput());
#endif
}

#ifndef _WIN32

TEST_F(SubprocessTest, InterruptChild) {
  Subprocess* subproc = subprocs_.Add("kill -INT $$");
  ASSERT_NE(static_cast<Subprocess*>(NULL), subproc);

  while (!subproc->Done()) {
    subprocs_.DoWork();
  }

  EXPECT_EQ(ExitInterrupted, subproc->Finish());
}

TEST_F(SubprocessTest, InterruptParent) {
  Subprocess* subproc = subprocs_.Add("kill -INT $PPID ; sleep 1");
  ASSERT_NE(static_cast<Subprocess*>(NULL), subproc);

  while (!subproc->Done()) {
    bool interrupted = subprocs_.DoWork();
    if (interrupted)
      return;
  }

  ADD_FAILURE() << "We should have been interrupted";
}

#endif

TEST_F(SubprocessTest, SetWithSingle) {
  Subprocess* subproc = subprocs_.Add(kSimpleCommand);
  ASSERT_NE(static_cast<Subprocess*>(NULL), subproc);

  while (!subproc->Done()) {
    subprocs_.DoWork();
  }
  ASSERT_EQ(ExitSuccess, subproc->Finish());
  ASSERT_NE(L"", subproc->GetOutput());

  ASSERT_EQ(1u, subprocs_.finished_.size());
}

TEST_F(SubprocessTest, SetWithMulti) {
  Subprocess* processes[3];
  const wchar_t* kCommands[3] = {
    kSimpleCommand,
#ifdef _WIN32
    L"cmd /c echo hi",
    L"cmd /c time /t",
#else
    "whoami",
    "pwd",
#endif
  };

  for (int i = 0; i < 3; ++i) {
    processes[i] = subprocs_.Add(kCommands[i]);
    ASSERT_NE(static_cast<Subprocess*>(NULL), processes[i]);
  }

  ASSERT_EQ(3u, subprocs_.running_.size());
  for (int i = 0; i < 3; ++i) {
    ASSERT_FALSE(processes[i]->Done());
    ASSERT_EQ(L"", processes[i]->GetOutput());
  }

  while (!processes[0]->Done() || !processes[1]->Done() ||
         !processes[2]->Done()) {
    ASSERT_GT(subprocs_.running_.size(), 0u);
    subprocs_.DoWork();
  }

  ASSERT_EQ(0u, subprocs_.running_.size());
  ASSERT_EQ(3u, subprocs_.finished_.size());

  for (int i = 0; i < 3; ++i) {
    ASSERT_EQ(ExitSuccess, processes[i]->Finish());
    ASSERT_NE(L"", processes[i]->GetOutput());
    delete processes[i];
  }
}

// OS X's process limit is less than 1025 by default
// (|sysctl kern.maxprocperuid| is 709 on 10.7 and 10.8 and less prior to that).
#ifdef linux
TEST_F(SubprocessTest, SetWithLots) {
  // Arbitrary big number; needs to be over 1024 to confirm we're no longer
  // hostage to pselect.
  const size_t kNumProcs = 1025;

  // Make sure [ulimit -n] isn't going to stop us from working.
  rlimit rlim;
  ASSERT_EQ(0, getrlimit(RLIMIT_NOFILE, &rlim));
  ASSERT_GT(rlim.rlim_cur, kNumProcs)
      << "Raise [ulimit -n] well above " << kNumProcs
      << " to make this test go";

  vector<Subprocess*> procs;
  for (size_t i = 0; i < kNumProcs; ++i) {
    Subprocess* subproc = subprocs_.Add("/bin/echo");
    ASSERT_NE(static_cast<Subprocess*>(NULL), subproc);
    procs.push_back(subproc);
  }
  while (!subprocs_.running_.empty())
    subprocs_.DoWork();
  for (size_t i = 0; i < procs.size(); ++i) {
    ASSERT_EQ(ExitSuccess, procs[i]->Finish());
    ASSERT_NE("", procs[i]->GetOutput());
  }
  ASSERT_EQ(kNumProcs, subprocs_.finished_.size());
}
#endif  // linux