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

// Read output from gdb, and notify upstream.
class OutputReader : public MessageLoopForIO::IOHandler {
 public:
  OutputReader(HANDLE file) : file_(file), terminating_(false) {
    memset(&context_, 0, sizeof(context_));
    context_.handler = this;
    MessageLoopForIO::current()->RegisterIOHandler(file, this);
    read_is_pending_ = false;
    ReadOutput();
  }

  virtual ~OutputReader() {
  }

  // Implementation of IOHandler:
  virtual void OnIOCompleted(MessageLoopForIO::IOContext* context,
                             DWORD bytes_transferred, DWORD error) {
    read_is_pending_ = false;
    unused_data_ = unused_data_ + std::string(buffer_, bytes_transferred);
#ifndef NDEBUG
    memset(buffer_, 0xcc, sizeof(buffer_));
#endif
    int bytes_consumed;
    scoped_ptr<GdbOutput> output(
        gdb_mi_reader_.Parse(unused_data_, &bytes_consumed));
    if (output.get()) {
      unused_data_ = unused_data_.substr(bytes_consumed);
      SendNotifications(output.get());
    }
    // TODO(scottmg): PostTask?
    if (!terminating_)
      ReadOutput();
  }

  void SetDebugNotification(DebugNotification* debug_notification) {
    debug_notification_ = debug_notification;
  }

  void SendNotifications(GdbOutput* output) {
    NOTIMPLEMENTED() << "todo";
  }

  void ReadOutput() {
    DWORD bytes_read;
    fprintf(stderr, "Starting Read\n");
    read_is_pending_ = true;
    BOOL result = ReadFile(
        file_, buffer_, sizeof(buffer_), &bytes_read, &context_.overlapped);
    if (!result)
      CHECK(ERROR_IO_PENDING == GetLastError());
  }

  void StopReading() {
    terminating_ = true;
  }
  bool ReadIsPending() {
    return read_is_pending_;
  }

 private:
  char buffer_[4 << 10];
  MessageLoopForIO::IOContext context_;
  HANDLE file_;
  DebugNotification* debug_notification_;
  GdbMiReader gdb_mi_reader_;
  std::string unused_data_;
  bool terminating_;
  bool read_is_pending_;
};

// Write commands to gdb.
class InputSender : public MessageLoopForIO::IOHandler {
 public:
  InputSender(HANDLE file)
      : file_(file), write_in_progress_(false), bytes_to_send_(0) {
    memset(&context_, 0, sizeof(context_));
    context_.handler = this;
    MessageLoopForIO::current()->RegisterIOHandler(file, this);
  }

  virtual ~InputSender() {
  }

  void CancelOutstandingAndSendString(const string16& string) {
    if (write_in_progress_) {
      MessageLoopForIO::current()->WaitForIOCompletion(INFINITE, this);
      write_in_progress_ = false;
    }
    pending_.clear();
    SendString(string);
  }

  void SendString(const string16& string) {
    if (write_in_progress_) {
      pending_.push_back(string);
    } else {
      std::string narrow = UTF16ToUTF8(string);
      CHECK(narrow.size() <= sizeof(buffer_));
#ifndef NDEBUG
      memset(buffer_, 0xcc, sizeof(buffer_));
#endif
      memcpy(buffer_, narrow.data(), narrow.size());
      write_in_progress_ = true;
      DWORD bytes_written;
      fprintf(stderr, "Starting Write\n");
      BOOL result = WriteFile(
          file_, buffer_, narrow.size(), &bytes_written, &context_.overlapped);
      bytes_to_send_ = narrow.size();
      if (!result)
        CHECK(GetLastError() == ERROR_IO_PENDING);
    }
  }

  virtual void OnIOCompleted(MessageLoopForIO::IOContext* context,
                             DWORD bytes_transferred, DWORD error) {
    // Write failed, probably terminating. 
    if (error != 0)
      return;
    CHECK(bytes_transferred == bytes_to_send_);
#ifndef NDEBUG
    memset(buffer_, 0xcc, sizeof(buffer_));
#endif
    write_in_progress_ = false;
    if (!pending_.empty()) {
      string16 to_send = pending_.front();
      pending_.pop_front();
      SendString(to_send);
    }
  }

  bool WriteIsPending() {
    return write_in_progress_;
  }

 private:
  char buffer_[4 << 10];
  MessageLoopForIO::IOContext context_;
  HANDLE file_;
  std::list<string16> pending_;
  bool write_in_progress_;
  int bytes_to_send_;
};

DebugCoreGdb::DebugCoreGdb() {
  CHECK(gdb_.Start(L"gdb_win_binaries/gdb-python27.exe",
                   L"--fullname -nx --interpreter=mi2 --quiet"));
  output_reader_.reset(new OutputReader(gdb_.GetInputPipe()));
  input_sender_.reset(new InputSender(gdb_.GetOutputPipe()));
}

DebugCoreGdb::~DebugCoreGdb() {
  output_reader_->StopReading();
  // TODO(scottmg): Need to move reader and writer into same completion so we
  // can block cleanly here on shutdown. see e.g. ipc/ipc_channel_win.cc
  /*
  while (output_reader_->ReadIsPending() || input_sender_->WriteIsPending()) {
    MessageLoopForIO::current()->WaitForIOCompletion(
        INFINITE, output_reader_.get());
    MessageLoopForIO::current()->WaitForIOCompletion(
        INFINITE, input_sender_.get());
  }
  */
  gdb_.Terminate();
  input_sender_.reset();
  output_reader_.reset();
}

void DebugCoreGdb::DeleteSelf() {
  fprintf(stderr, "deleting\n");
  delete this;
  fprintf(stderr, "done deleting\n");
}

void DebugCoreGdb::SendCommand(const string16& arg0) {
  string16 command = arg0 + L"\r\n";
  input_sender_->SendString(command);
}

void DebugCoreGdb::SendCommand(const string16& arg0, const string16& arg1) {
  string16 command = arg0 + L" " + arg1 + L"\r\n";
  input_sender_->SendString(command);
}

void DebugCoreGdb::SendCommand(const string16& arg0,
                               const string16& arg1,
                               const string16& arg2) {
  string16 command = arg0 + L" " + arg1 + L" " + arg2 + L"\r\n";
  input_sender_->SendString(command);
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
  output_reader_->SetDebugNotification(debug_notification);
}

void DebugCoreGdb::Start() {
}


base::WeakPtr<DebugCoreGdb> DebugCoreGdb::Create() {
  DebugCoreGdb* result = new DebugCoreGdb;
  return result->AsWeakPtr();
}
