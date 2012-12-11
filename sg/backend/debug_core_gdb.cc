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
#include "sg/backend/gdb_mi_parse.h"

// Handles async reads and writes to subprocess. Read and write on the same
// object to simplify blocking on shutdown.
class ReaderWriter : public MessageLoopForIO::IOHandler {
 public:
  ReaderWriter(HANDLE input, HANDLE output)
      : read_state_(input),
        write_state_(output),
        terminating_(false),
        debug_notification_(NULL) {
    MessageLoopForIO::current()->RegisterIOHandler(input, this);
    MessageLoopForIO::current()->RegisterIOHandler(output, this);
    read_state_.context.handler = this;
    write_state_.context.handler = this;
    StartRead();
  }

  virtual ~ReaderWriter() {
    // Prevent read from restarting.
    terminating_ = true;
    pending_writes_.clear();
    CancelIo(read_state_.file);
    CancelIo(write_state_.file);
    while (read_state_.is_pending || write_state_.is_pending)
      MessageLoopForIO::current()->WaitForIOCompletion(INFINITE, this);
  }

  // Implementation of IOHandler:
  virtual void OnIOCompleted(MessageLoopForIO::IOContext* context,
                             DWORD bytes_transferred, DWORD error) {
    if (context == &read_state_.context) {
      read_state_.is_pending = false;
      CompleteRead(bytes_transferred);
    } else {
      CHECK(context == &write_state_.context);
      write_state_.is_pending = false;
      CompleteWrite(bytes_transferred);
    }
  }

  void CompleteRead(DWORD bytes_transferred) {
    unused_read_data_ += std::string(read_state_.buffer, bytes_transferred);
#ifndef NDEBUG
    memset(read_state_.buffer, 0xcc, sizeof(read_state_.buffer));
#endif
    int bytes_consumed;
    scoped_ptr<GdbOutput> output(
        gdb_mi_reader_.Parse(unused_read_data_, &bytes_consumed));
    if (output.get()) {
      unused_read_data_ = unused_read_data_.substr(bytes_consumed);
      SendNotifications(output.get());
    }
    // TODO(scottmg): PostTask?
    if (!terminating_)
      StartRead();
  }

  void CompleteWrite(DWORD bytes_transferred) {
#ifndef NDEBUG
    memset(write_state_.buffer, 0xcc, sizeof(write_state_.buffer));
#endif
    if (!pending_writes_.empty()) {
      string16 to_send = pending_writes_.front();
      pending_writes_.pop_front();
      SendString(to_send);
    }
  }

  void SetDebugNotification(DebugNotification* debug_notification) {
    debug_notification_ = debug_notification;
  }

  void SendNotifications(GdbOutput* output) {
    NOTIMPLEMENTED() << "todo";
  }

  void StartRead() {
    read_state_.is_pending = true;
    DWORD bytes_read;
    BOOL result = ReadFile(
        read_state_.file, read_state_.buffer, sizeof(read_state_.buffer),
        &bytes_read, &read_state_.context.overlapped);
    if (!result)
      CHECK(ERROR_IO_PENDING == GetLastError());
  }

  void SendString(const string16& string) {
    if (write_state_.is_pending) {
      pending_writes_.push_back(string);
    } else {
      std::string narrow = UTF16ToUTF8(string);
      CHECK(narrow.size() <= sizeof(write_state_.buffer));
#ifndef NDEBUG
      memset(write_state_.buffer, 0xcc, sizeof(write_state_.buffer));
#endif
      memcpy(write_state_.buffer, narrow.data(), narrow.size());
      write_state_.is_pending = true;
      DWORD bytes_written;
      BOOL result = WriteFile(
          write_state_.file, write_state_.buffer, narrow.size(),
          &bytes_written, &write_state_.context.overlapped);
      if (!result)
        CHECK(GetLastError() == ERROR_IO_PENDING);
    }
  }

 private:
  struct State {
    explicit State(HANDLE file) : is_pending(false), file(file) {
      memset(&context, 0, sizeof(context));
    }
    MessageLoopForIO::IOContext context;
    char buffer[4 << 10];
    bool is_pending;
    HANDLE file;
  };

  State read_state_;
  State write_state_;

  GdbMiReader gdb_mi_reader_;
  std::string unused_read_data_;

  std::list<string16> pending_writes_;

  bool terminating_;

  DebugNotification* debug_notification_;
};

DebugCoreGdb::DebugCoreGdb() {
  CHECK(gdb_.Start(L"gdb_win_binaries/gdb-python27.exe",
                   L"--fullname -nx --interpreter=mi2 --quiet"));
  reader_writer_.reset(new ReaderWriter(
        gdb_.GetInputPipe(), gdb_.GetOutputPipe()));
}

DebugCoreGdb::~DebugCoreGdb() {
  reader_writer_.reset();
  // TODO(scottmg): Send a -gdb-quit and Finish() cleanly instead.
  gdb_.Terminate();
}

void DebugCoreGdb::DeleteSelf() {
  delete this;
}

void DebugCoreGdb::SendCommand(const string16& arg0) {
  string16 command = arg0 + L"\r\n";
  reader_writer_->SendString(command);
}

void DebugCoreGdb::SendCommand(const string16& arg0, const string16& arg1) {
  string16 command = arg0 + L" " + arg1 + L"\r\n";
  reader_writer_->SendString(command);
}

void DebugCoreGdb::SendCommand(const string16& arg0,
                               const string16& arg1,
                               const string16& arg2) {
  string16 command = arg0 + L" " + arg1 + L" " + arg2 + L"\r\n";
  reader_writer_->SendString(command);
}

void DebugCoreGdb::LoadProcess(
    const string16& application,
    const string16& command_line,
    const std::vector<string16> environment,
    const string16& working_directory) {
  DCHECK_EQ(0, environment.size()) << "todo;";
  DCHECK_EQ(0, working_directory.size()) << "todo;";
  // TODO(scottmg) Any need to post this? The WriteFile is async, and we're
  // already on the DBG thread, so should be OK I think.
  SendCommand(L"-file-exec-and-symbols", application);
  SendCommand(L"-break-insert", L"-t", L"main");
  SendCommand(L"-exec-run");
}

void DebugCoreGdb::StopDebugging() {
  // TODO(scottmg): No MI command -exec it seems.
  SendCommand(L"kill");
}

void DebugCoreGdb::SetDebugNotification(DebugNotification* debug_notification) {
  reader_writer_->SetDebugNotification(debug_notification);
}

void DebugCoreGdb::Start() {
}


base::WeakPtr<DebugCoreGdb> DebugCoreGdb::Create() {
  DebugCoreGdb* result = new DebugCoreGdb;
  return result->AsWeakPtr();
}
