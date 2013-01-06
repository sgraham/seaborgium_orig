// Copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef SG_BACKEND_BACKEND_H_
#define SG_BACKEND_BACKEND_H_

#include <vector>

#include "base/string16.h"

class TypeNameValue {
 public:
  string16 type;
  string16 name;
  string16 value;
};

class FrameData {
 public:
  uintptr_t address;
  string16 function;
  string16 filename;
  int line_number;
  std::vector<TypeNameValue> arguments;
};

class StoppedAtBreakpointData {
 public:
  FrameData frame;
};

class LibraryLoadedData {
 public:
  string16 host_path;
  string16 target_path;
  string16 thread_id;
  bool symbols_loaded;
};

class LibraryUnloadedData {
 public:
  string16 host_path;
  string16 target_path;
  string16 thread_id;
};

class StoppedAfterSteppingData {
 public:
  FrameData frame;
};

class RetrievedStackData {
 public:
  std::vector<FrameData> frames;
};

class RetrievedLocalsData {
 public:
  std::vector<string16> local_names;
};

class WatchCreatedData {
 public:
  std::string variable_id;
  bool has_children;
  string16 value;
  string16 type;
};

class WatchesUpdatedData {
 public:
  struct Item {
    std::string variable_id;
    string16 value;
    bool type_changed;
  };
  std::vector<Item> watches;
};

class WatchesChildListData {
 public:
  struct Child {
    std::string variable_id;
    string16 expression;
    string16 value;
    string16 type;
  };
  std::string parent;
  std::vector<Child> children;
};

class DebugNotification {
 public:
  virtual ~DebugNotification() {}

  virtual void OnProcessLoaded() {}
  virtual void OnStoppedAtBreakpoint(const StoppedAtBreakpointData& data) {}
  virtual void OnStoppedAfterStepping(const StoppedAfterSteppingData& data) {}
  virtual void OnLibraryLoaded(const LibraryLoadedData& data) {}
  virtual void OnLibraryUnloaded(const LibraryUnloadedData& data) {}
  virtual void OnRetrievedStack(const RetrievedStackData& data) {}
  virtual void OnRetrievedLocals(const RetrievedLocalsData& data) {}
  virtual void OnWatchCreated(const WatchCreatedData& data) {}
  virtual void OnWatchesUpdated(const WatchesUpdatedData& data) {}
  virtual void OnWatchChildList(const WatchesChildListData& data) {}
  virtual void OnConsoleOutput(const string16& data) {}
  virtual void OnInternalDebugOutput(const string16& data) {}
};

// TODO: Generic-ize debug_core_gdb to here.

#endif  // SG_BACKEND_BACKEND_H_
