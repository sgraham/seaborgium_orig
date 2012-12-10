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

#include <gtest/gtest.h>

#include "base/bind.h"
#include "base/message_loop.h"
#include "base/threading/thread.h"
#include "base/time.h"

namespace {

#ifdef _WIN32
const wchar_t* kSimpleCommand = L"cmd /c dir \\";
#else
const char* kSimpleCommand = "ls /";
#endif

}  // anonymous namespace

TEST(SubprocessTest, SpawnSuccessfully) {
  Subprocess subproc;
  subproc.Start(_wgetenv(L"COMSPEC"), L"/c dir \\");
}

/*
TEST(SubprocessTest, StartupRace) {
  Subprocess subproc;
  subproc.Start(L"gdb_win_binaries/gdb-python27.exe",
                L"--interpreter=mi2 --quiet");
  char buffer[4096];
  DWORD num_read;
  Sleep(500);
  bool result = ReadFile(
      subproc.GetInputPipe(), buffer, sizeof(buffer), &num_read, NULL);
  EXPECT_TRUE(result);
  EXPECT_EQ("=thread-group-added,id=\"i1\"\r(gdb)\r", std::string(buffer));
}
*/

class TestIOHandler : public MessageLoopForIO::IOHandler {
 public:
  TestIOHandler(HANDLE input, HANDLE signal, const std::string& expected_data);

  virtual void OnIOCompleted(MessageLoopForIO::IOContext* context,
                             DWORD bytes_transfered, DWORD error);

  void Init();
  OVERLAPPED* context() { return &context_.overlapped; }
  DWORD size() { return sizeof(buffer_); }

 private:
  char buffer_[4 << 10];
  MessageLoopForIO::IOContext context_;
  HANDLE signal_;
  HANDLE file_;
  std::string expected_data_;
};

TestIOHandler::TestIOHandler(
    HANDLE input, HANDLE signal, const std::string& expected_data)
    : signal_(signal), expected_data_(expected_data) {
  memset(buffer_, 0, sizeof(buffer_));
  memset(&context_, 0, sizeof(context_));
  context_.handler = this;

  file_ = input;
}

void TestIOHandler::Init() {
  MessageLoopForIO::current()->RegisterIOHandler(file_, this);

  DWORD read;
  EXPECT_FALSE(ReadFile(file_, buffer_, size(), &read, context()));
  EXPECT_EQ(ERROR_IO_PENDING, GetLastError());
}

void TestIOHandler::OnIOCompleted(MessageLoopForIO::IOContext* context,
                                  DWORD bytes_transfered, DWORD error) {
  ASSERT_TRUE(context == &context_);
  ASSERT_TRUE(SetEvent(signal_));
  EXPECT_EQ(expected_data_, std::string(buffer_));
}

TEST(SubprocessTest, IOHandler) {
  base::win::ScopedHandle callback_called(CreateEvent(NULL, TRUE, FALSE, NULL));
  ASSERT_TRUE(callback_called.IsValid());

  Subprocess subproc;

  base::Thread thread("IOHandler test");
  base::Thread::Options options;
  options.message_loop_type = MessageLoop::TYPE_IO;
  ASSERT_TRUE(thread.StartWithOptions(options));

  MessageLoop* thread_loop = thread.message_loop();
  ASSERT_TRUE(NULL != thread_loop);

  TestIOHandler handler(
      subproc.GetInputPipe(),
      callback_called,
      "=thread-group-added,id=\"i1\"\r\n");
  thread_loop->PostTask(FROM_HERE, base::Bind(&TestIOHandler::Init,
                                              base::Unretained(&handler)));
  // Make sure the thread runs and sleeps for lack of work.
  base::PlatformThread::Sleep(base::TimeDelta::FromMilliseconds(100));

  // Now, start the subprocess and make sure we get its initial output.
  subproc.Start(L"gdb_win_binaries/gdb-python27.exe",
                L"--interpreter=mi2 -ex quit");

  DWORD result = WaitForSingleObject(callback_called, 1000);
  EXPECT_EQ(WAIT_OBJECT_0, result);

  thread.Stop();
}
/*
// Run a command that fails and emits to stderr.
TEST_F(SubprocessTest, BadCommandStderr) {
  Subprocess* subproc = subprocs_.Add(L"cmd /c ninja_no_such_command");
  Subprocess* 
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
*/
