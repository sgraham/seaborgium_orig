// Copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef SG_BACKEND_DEBUG_CORE_GDB_H_
#define SG_BACKEND_DEBUG_CORE_GDB_H_

#include <string>
#include <vector>

#include "base/memory/weak_ptr.h"
#include "base/message_loop.h"
#include "base/threading/non_thread_safe.h"
#include "sg/backend/backend.h"
#include "sg/backend/subprocess.h"

// An implementation of a debugger backend using GDB/MI.

// GDB does not read any input when it wishes to send output, and it does not
// do any buffering. So, we keep an async read always queued on its output
// stream, and process the records as it sends them. And, design the visible
// interface from this level to be async, so when we write commands to gdb we
// can assume there's no need to coordinate with the output it's giving us.

class GdbRecord;
class ReaderWriter;

typedef base::Callback<void (const GdbRecord*)> RecordHandler;

class DebugCoreGdb : public base::NonThreadSafe,
                     public base::SupportsWeakPtr<DebugCoreGdb> {
                     /*, public DebugCore*/
 public:
  DebugCoreGdb();
  virtual ~DebugCoreGdb();

  // Implementation of DebugCore:
  virtual void SetDebugNotification(DebugNotification* debug_notification);

  virtual void LoadProcess(
      const string16& application,
      const string16& command_line,
      const std::vector<string16> environment,
      const string16& working_directory);

  virtual void RunToMain();
  virtual void Continue();
  virtual void StepOver();
  virtual void StepIn();
  virtual void StepOut();

  virtual void StopDebugging();

  virtual void GetStack();
  virtual void GetLocals();
  virtual void UpdateWatches();
  virtual void SetWatchExpanded(const std::string& id, bool expanded);

  // |id| should be created via GenerateNewVariableIdentifier.
  virtual void CreateWatch(const std::string& id, const string16& name);
  virtual void DeleteWatch(const std::string& id);

  void DeleteSelf();

  static base::WeakPtr<DebugCoreGdb> Create();

 private:
  void SendCommand(const string16& arg0);
  void SendCommand(const string16& arg0, const string16& arg1);
  void SendCommand(
      const string16& arg0, const string16& arg1, const string16& arg2);
  void SendCommand(
      const string16& arg0,
      const string16& arg1,
      const string16& arg2,
      const string16& arg3);

  void SendCommand(int64 token, RecordHandler handler, const string16& arg0);
  void SendCommand(int64 token, RecordHandler handler, const string16& arg0,
                   const string16& arg1);
  void SendCommand(int64 token, RecordHandler handler, const string16& arg0,
                   const string16& arg1, const string16& arg2);
  void SendCommand(int64 token, RecordHandler handler, const string16& arg0,
                   const string16& arg1, const string16& arg2,
                   const string16& arg3);

  int64 NewToken();

  DebugNotification* debug_notification_;
  Subprocess gdb_;
  scoped_ptr<ReaderWriter> reader_writer_;
  int64 token_;

  DISALLOW_COPY_AND_ASSIGN(DebugCoreGdb);
};

#endif  // SG_BACKEND_DEBUG_CORE_GDB_H_
