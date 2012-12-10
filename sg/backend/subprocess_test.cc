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

  void DoRead();
  OVERLAPPED* context() { return &context_.overlapped; }
  DWORD size() { return sizeof(buffer_); }

  void DoWrite();

 private:
  char buffer_[4 << 10];
  MessageLoopForIO::IOContext context_;
  HANDLE signal_;
  HANDLE file_;
  std::string expected_data_;
  bool reading_;
};

// Can't use GDB for these tests because it expects a rigid
// read-then-write-then-read (or whatever), so we use a helper binary that
// does controllable reads and writes corresponding to the test.

TestIOHandler::TestIOHandler(
    HANDLE handle, HANDLE signal, const std::string& expected_data)
    : signal_(signal), expected_data_(expected_data) {
  memset(buffer_, 0, sizeof(buffer_));
  memset(&context_, 0, sizeof(context_));
  context_.handler = this;
  reading_ = true;

  file_ = handle;
}

void TestIOHandler::DoRead() {
  MessageLoopForIO::current()->RegisterIOHandler(file_, this);

  DWORD read;
  EXPECT_FALSE(ReadFile(file_, buffer_, size(), &read, context()));
  EXPECT_TRUE(ERROR_IO_PENDING == GetLastError() ||
              ERROR_BROKEN_PIPE == GetLastError()); // Pipe closed already.
  reading_ = true;
}

void TestIOHandler::DoWrite() {
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
  EXPECT_EQ(0, error);
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
      "some stuff\r\n");
  thread_loop->PostTask(FROM_HERE, base::Bind(&TestIOHandler::DoRead,
                                              base::Unretained(&handler)));
  // Now, start the subprocess and make sure we get its initial output (have
  // it quit immediately).
  subproc.Start(L"out/reader_writer_test.exe",
                L"w");

  DWORD result = WaitForSingleObject(callback_called, 1000);
  EXPECT_EQ(WAIT_OBJECT_0, result);

  thread.Stop();
}

TEST(SubprocessTest, WriteViaIOHandler) {
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
      subproc.GetOutputPipe(),
      callback_called,
      "quit\r\n");
  thread_loop->PostTask(FROM_HERE, base::Bind(&TestIOHandler::DoWrite,
                                              base::Unretained(&handler)));

  CHECK(subproc.Start(L"out/reader_writer_test.exe", L"r"));

  DWORD result = WaitForSingleObject(callback_called, INFINITE);
  EXPECT_EQ(WAIT_OBJECT_0, result);

  thread.Stop();
}

TEST(SubprocessTest, ReadAndWrite) {
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
      "some stuff\r\n");
  thread_loop->PostTask(FROM_HERE, base::Bind(&TestIOHandler::DoRead,
                                              base::Unretained(&handler)));

  TestIOHandler handler2(
      subproc.GetOutputPipe(),
      write_finished,
      "quit\r\n");
  thread_loop->PostTask(FROM_HERE, base::Bind(&TestIOHandler::DoWrite,
                                              base::Unretained(&handler2)));

  CHECK(subproc.Start(L"out/reader_writer_test.exe", L"wr"));

  HANDLE handles[] = { read_finished, write_finished };
  DWORD result = WaitForMultipleObjects(2, handles, TRUE, INFINITE);
  EXPECT_EQ(WAIT_OBJECT_0, result);

  thread.Stop();
}
