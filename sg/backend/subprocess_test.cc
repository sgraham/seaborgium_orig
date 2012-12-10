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

TEST(SubprocessTest, SpawnSuccessfully) {
  Subprocess subproc;
  EXPECT_EQ(true, subproc.Start(_wgetenv(L"COMSPEC"), L"/c dir \\"));
}

class TestIOHandler : public MessageLoopForIO::IOHandler {
 public:
  TestIOHandler(HANDLE handle, HANDLE signal, const std::string& expected_data);

  virtual void OnIOCompleted(MessageLoopForIO::IOContext* context,
                             DWORD bytes_transfered, DWORD error);

  void Init();
  OVERLAPPED* context() { return &context_.overlapped; }
  DWORD size() { return sizeof(buffer_); }

  void WriteString();

 private:
  char buffer_[4 << 10];
  MessageLoopForIO::IOContext context_;
  HANDLE signal_;
  HANDLE file_;
  std::string expected_data_;
  bool reading_;
};

TestIOHandler::TestIOHandler(
    HANDLE handle, HANDLE signal, const std::string& expected_data)
    : signal_(signal), expected_data_(expected_data) {
  memset(buffer_, 0, sizeof(buffer_));
  memset(&context_, 0, sizeof(context_));
  context_.handler = this;
  reading_ = true;

  file_ = handle;
}

void TestIOHandler::Init() {
  MessageLoopForIO::current()->RegisterIOHandler(file_, this);

  DWORD read;
  EXPECT_FALSE(ReadFile(file_, buffer_, size(), &read, context()));
  EXPECT_TRUE(ERROR_IO_PENDING == GetLastError() ||
              ERROR_BROKEN_PIPE == GetLastError()); // Pipe closed already.
  reading_ = true;
}

void TestIOHandler::WriteString() {
  MessageLoopForIO::current()->RegisterIOHandler(file_, this);

  DWORD written;
  EXPECT_FALSE(
      ::WriteFile(file_, expected_data_.data(),
                  expected_data_.size(), &written, context()));
  EXPECT_EQ(ERROR_IO_PENDING, GetLastError());
  reading_ = false;
}

void TestIOHandler::OnIOCompleted(MessageLoopForIO::IOContext* context,
                                  DWORD bytes_transfered, DWORD error) {
  ASSERT_TRUE(context == &context_);
  ASSERT_TRUE(SetEvent(signal_));
  if (reading_)
    EXPECT_EQ(expected_data_, std::string(buffer_));
}

TEST(SubprocessTest, ReadViaIOHandler) {
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

  // Now, start the subprocess and make sure we get its initial output (have
  // it quit immediately).
  subproc.Start(L"gdb_win_binaries/gdb-python27.exe",
                L"--interpreter=mi2 -ex q");

  DWORD result = WaitForSingleObject(callback_called, 1000);
  EXPECT_EQ(WAIT_OBJECT_0, result);

  thread.Stop();
}

TEST(SubprocessTest, DISABLED_ReadAndWrite) {
  base::win::ScopedHandle read_finished(CreateEvent(NULL, TRUE, FALSE, NULL));
  ASSERT_TRUE(read_finished.IsValid());
  base::win::ScopedHandle write_finished(CreateEvent(NULL, TRUE, FALSE, NULL));
  ASSERT_TRUE(write_finished.IsValid());

  Subprocess subproc;

  base::Thread thread("IOHandler test");
  base::Thread::Options options;
  options.message_loop_type = MessageLoop::TYPE_IO;
  ASSERT_TRUE(thread.StartWithOptions(options));

  MessageLoop* thread_loop = thread.message_loop();
  ASSERT_TRUE(NULL != thread_loop);

  TestIOHandler handler(
      subproc.GetInputPipe(),
      read_finished,
      "=thread-group-added,id=\"i1\"\r\n");
  thread_loop->PostTask(FROM_HERE, base::Bind(&TestIOHandler::Init,
                                              base::Unretained(&handler)));

  // Now, start the subprocess.
  subproc.Start(L"gdb_win_binaries/gdb-python27.exe",
                L"--interpreter=mi2");

  base::PlatformThread::Sleep(base::TimeDelta::FromMilliseconds(100));

  TestIOHandler handler2(
      subproc.GetOutputPipe(),
      write_finished,
      "quit\r\n");
  thread_loop->PostTask(FROM_HERE, base::Bind(&TestIOHandler::WriteString,
                                              base::Unretained(&handler)));

  HANDLE handles[] = { read_finished, write_finished };
  DWORD result = WaitForMultipleObjects(2, handles, TRUE, INFINITE);
  EXPECT_EQ(WAIT_OBJECT_0, result);

  thread.Stop();
}
