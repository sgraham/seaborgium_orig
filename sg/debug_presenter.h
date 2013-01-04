// Copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef SG_DEBUG_PRESENTER_H_
#define SG_DEBUG_PRESENTER_H_

#include <map>
#include <string>

#include "base/basictypes.h"
#include "base/file_path.h"
#include "base/memory/weak_ptr.h"
#include "sg/debug_presenter_notify.h"
#include "sg/backend/backend.h"

class DebugCoreGdb;
class DebugPresenterDisplay;
class SourceFiles;

class DebugPresenter : public DebugPresenterNotify,
                       public DebugNotification {
 public:
  explicit DebugPresenter(SourceFiles* source_files);
  virtual ~DebugPresenter();

  virtual void SetDisplay(DebugPresenterDisplay* display);
  virtual void SetDebugCore(base::WeakPtr<DebugCoreGdb> debug_core);

  // Implementation of DebugPresenterNotify.
  virtual void NotifyFramePainted(double frame_time_in_ms) OVERRIDE;
  virtual bool NotifyKey(
      InputKey key, bool down, const InputModifiers& modifiers) OVERRIDE;

  // Implementation of DebugNotification
  virtual void OnStoppedAtBreakpoint(
      const StoppedAtBreakpointData& data) OVERRIDE;
  virtual void OnStoppedAfterStepping(
      const StoppedAfterSteppingData& data) OVERRIDE;
  virtual void OnLibraryLoaded(const LibraryLoadedData& data) OVERRIDE;
  virtual void OnLibraryUnloaded(const LibraryUnloadedData& data) OVERRIDE;
  virtual void OnRetrievedStack(const RetrievedStackData& data) OVERRIDE;
  virtual void OnRetrievedLocals(const RetrievedLocalsData& data) OVERRIDE;
  virtual void OnWatchCreated(const WatchCreatedData& data) OVERRIDE;
  virtual void OnConsoleOutput(const string16& data) OVERRIDE;
  virtual void OnInternalDebugOutput(const string16& data) OVERRIDE;

 private:
  void ReadFileOnFILE(FilePath path, std::string* result);
  void FileLoadCompleted(FilePath path, std::string* result);

  // On stopping, starts requests for stack, values of locals, etc.
  void UpdatePassiveDisplays();

  std::string GenerateNewVariableIdentifier();

  string16 binary_;
  DebugPresenterDisplay* display_;
  SourceFiles* source_files_;
  base::WeakPtr<DebugCoreGdb> debug_core_;

  int64 variable_counter_;

  // Map from Type+Name+Value to variable id on the backend.
  std::map<string16, std::string> backend_variables_;

  bool running_;

  DISALLOW_COPY_AND_ASSIGN(DebugPresenter);
};

#endif  // SG_DEBUG_PRESENTER_H_
