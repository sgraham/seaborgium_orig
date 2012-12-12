// Copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "sg/debug_presenter.h"

#include "base/bind.h"
#include "base/command_line.h"
#include "base/file_util.h"
#include "sg/app_thread.h"
#include "sg/backend/debug_core_gdb.h"
#include "sg/debug_presenter_display.h"
#include "sg/source_files.h"

DebugPresenter::DebugPresenter(SourceFiles* source_files)
    : source_files_(source_files) {
  const CommandLine& command_line = *CommandLine::ForCurrentProcess();
  // TODO(scottmg): Temporary obviously.
  if (command_line.GetArgs().size() != 1)
    binary_ = L"test_data/test_binary_mingw.exe";
  else
    binary_ = command_line.GetArgs()[0];
}

DebugPresenter::~DebugPresenter() {
}

void DebugPresenter::SetDisplay(DebugPresenterDisplay* display) {
  display_ = display;
}

void DebugPresenter::NotifyFramePainted(double frame_time_in_ms) {
  // Should really store this to some model.
  display_->SetRenderTime(frame_time_in_ms);
}

void DebugPresenter::ReadFileOnFILE(FilePath path, std::string* result) {
  file_util::ReadFileToString(path, result);
}

bool DebugPresenter::NotifyKey(
    InputKey key, bool down, const InputModifiers& modifiers) {
  if (key == kF10 && down && modifiers.None()) {
    AppThread::PostTask(AppThread::BACKEND, FROM_HERE,
        base::Bind(&DebugCoreGdb::StepOver,
                   debug_core_));
    return true;
  }
  else if (key == kF11 && down && modifiers.None()) {
    AppThread::PostTask(AppThread::BACKEND, FROM_HERE,
        base::Bind(&DebugCoreGdb::StepIn,
                   debug_core_));
    return true;
  }
  return false;
}

void DebugPresenter::FileLoadCompleted(
    FilePath path, std::string* result) {
  // TODO(scottmg): mtime.
  source_files_->SetFileData(path, 0, *result);
  display_->SetFileName(path);
  display_->SetFileData(*result);
  delete result;
}

void DebugPresenter::SetDebugCore(base::WeakPtr<DebugCoreGdb> debug_core) {
  debug_core_ = debug_core;
  AppThread::PostTask(AppThread::BACKEND, FROM_HERE,
      base::Bind(&DebugCoreGdb::SetDebugNotification,
                 debug_core_,
                 this));
  AppThread::PostTask(AppThread::BACKEND, FROM_HERE,
      base::Bind(&DebugCoreGdb::LoadProcess,
                 debug_core,
                 binary_,
                 L"", std::vector<string16>(), L""));
}

void DebugPresenter::OnStoppedAtBreakpoint(
    const StoppedAtBreakpointData& data) {
  std::string* result = new std::string;
  // TODO(scottmg): Need to relativize to binary location (or search in some
  // reasonable way anyway).
  FilePath path(string16(L"test_data/") + data.frame.filename);
  AppThread::PostTaskAndReply(AppThread::FILE, FROM_HERE,
    base::Bind(&DebugPresenter::ReadFileOnFILE,
               base::Unretained(this), path, result),
    base::Bind(&DebugPresenter::FileLoadCompleted,
               base::Unretained(this), path, result));
  display_->SetProgramCounterLine(data.frame.line_number);
}

void DebugPresenter::OnStoppedAfterStepping(
    const StoppedAfterSteppingData& data) {
  // TODO(scottmg): File change reload, etc.
  display_->SetProgramCounterLine(data.frame.line_number);
}
