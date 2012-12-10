// Copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "sg/backend/debug_core_gdb.h"

#include "base/bind.h"
#include "base/logging.h"
#include "base/memory/scoped_ptr.h"
#include "base/message_loop.h"
#include "base/string16.h"
#include "base/utf_string_conversions.h"

// So... no point in any async read or write because GDB only wants one or the
// other. Loop has to look like:
//
// for (;;) {
//   ReadUntilReady()
//   SendOneCommand()
// }
//
// So, start with PostTask(ReadUntilReady, this).
// If GDB has not stopped talking, repeat.
//
// Other threads can Post to us to queue to requested commands.
//
// If we change to kStateStopped, then take pop front of requested commands,
// WriteFile to gdb, and ReadUntilReady.

DebugCoreGdb::DebugCoreGdb() {
  CHECK(gdb_.Start(L"gdb_win_binaries/gdb-python27.exe",
                   L"--fullname -nx --interpreter=mi2 --quiet"));
  memset(&context_, 0, sizeof(context_));
  context_.handler = this;
  MessageLoopForIO::current()->RegisterIOHandler(gdb_.GetOutputPipe(), this);
  MessageLoopForIO::current()->PostTask(FROM_HERE,
    base::Bind(&DebugCoreGdb::ReadUntilReady,
               base::Unretained(this)));  // TODO(lifetime): WeakPtrFactory.
}

DebugCoreGdb::~DebugCoreGdb() {
}

void DebugCoreGdb::OnIOCompleted(
    MessageLoopForIO::IOContext* context,
    DWORD bytes_transfered, DWORD error) {
  receive_buffer_[bytes_transfered] = 0;
  printf("'%s'\n", receive_buffer_);
  if (strncmp(receive_buffer_, "(gdb) ", 6) == 0) {
    printf("XXX Done reading\n");
  } else {
    MessageLoopForIO::current()->PostTask(FROM_HERE,
      base::Bind(&DebugCoreGdb::ReadUntilReady,
                base::Unretained(this)));  // TODO(lifetime): WeakPtrFactory.
  }
}

void DebugCoreGdb::SendCommand(const string16& arg0, const string16& arg1) {
  string16 command = arg0 + L" " + arg1;
  std::string narrow = UTF16ToUTF8(command);
  memcpy(send_buffer_, narrow.data(), narrow.size());
  DWORD bytes_written;
  CHECK(false ==
      WriteFile(gdb_.GetOutputPipe(), send_buffer_, narrow.size(),
                &bytes_written, &context_.overlapped));
  CHECK(ERROR_IO_PENDING == GetLastError());
}

void DebugCoreGdb::ReadUntilReady() {
  DWORD bytes_read;
  BOOL result = ReadFile(gdb_.GetInputPipe(),
                         receive_buffer_, sizeof(receive_buffer_),
                         &bytes_read, &context_.overlapped);
  if (result) {
    OnIOCompleted(&context_, bytes_read, 0);
  } else {
    CHECK(ERROR_IO_PENDING == GetLastError());
  }
}

void DebugCoreGdb::LoadProcess(
    const string16& application,
    const string16& command_line,
    const std::vector<string16> environment,
    const string16& working_directory) {
  DCHECK_EQ(0, environment.size()) << "todo;";
  DCHECK_EQ(0, working_directory.size()) << "todo;";
  MessageLoopForIO::current()->PostTask(FROM_HERE,
      base::Bind(&DebugCoreGdb::SendCommand,
                 base::Unretained(this),  // TODO(lifetime): WeakPtrFactory.
                 L"-file-exec-and-symbols",
                 application));
  /*
  MessageLoopForIO::current()->PostTask(FROM_HERE,
      base::Bind(&DebugCoreGdb::SendCommand,
                 base::Unretained(this),  // TODO(lifetime): WeakPtrFactory.
                 L"-exec-arguments",
                 command_line));
                 */
}

void DebugCoreGdb::SetDebugNotification(DebugNotification* debug_notification) {
  debug_notification_ = debug_notification;
}

void DebugCoreGdb::Start() {
}


DebugCoreGdb* DebugCoreGdb::Create() {
  return new DebugCoreGdb;
}
