// Copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "sg/debug_presenter.h"

#include <algorithm>
#include <vector>

#include "base/bind.h"
#include "base/command_line.h"
#include "base/file_util.h"
#include "base/string_number_conversions.h"
#include "base/string_util.h"
#include "sg/app_thread.h"
#include "sg/backend/debug_core_gdb.h"
#include "sg/debug_presenter_display.h"
#include "sg/source_files.h"

// TODO(scottmg): Maintaining running_ here is probably wrong and going to
// cause pain. It should be semi-async updated from the backend somehow.

DebugPresenter::DebugPresenter(SourceFiles* source_files)
    : source_files_(source_files),
      variable_counter_(0),
      running_(false) {
  const CommandLine& command_line = *CommandLine::ForCurrentProcess();
  // TODO(scottmg): Temporary obviously.
  if (command_line.GetArgs().size() != 1)
    binary_ = L"test_data/test_binary_mingw.exe";
  else
    ReplaceChars(command_line.GetArgs()[0], L"\\", L"/", &binary_);
}

DebugPresenter::~DebugPresenter() {
}

void DebugPresenter::SetDisplay(DebugPresenterDisplay* display) {
  display_ = display;
  if (!running_)
    display_->SetFileData("\n"
                          "Binary from command line (or sample) loaded.\n"
                          "\n"
                          "Seaborgium ghetto quickstart:\n"
                          "\n"
                          "F10: step over (and run to main() on startup)\n"
                          "F11: step in\n"
                          "S-F11: step out\n"
                          // "F9: add/remove breakpoint\n"
                          // "C-F9: enable/disable breakpoint\n"
                          // "S-F9: edit breakpoint properties\n"
                          "F5: run (not too useful yet)\n"
                          "S-F5: stop debugging\n"
                          "C-S-F5: restart debugging\n"
                          "\n"
                          "Resize/redock windows with mouse\n");
}

void DebugPresenter::NotifyFramePainted(double frame_time_in_ms) {
  // Should really store this to some model.
  display_->SetRenderTime(frame_time_in_ms);
}

bool DebugPresenter::NotifyKey(
    InputKey key, bool down, const InputModifiers& modifiers) {
  // TODO(config): I guess.
  if (key == kF10 && down && modifiers.None()) {
    AppThread::PostTask(AppThread::BACKEND, FROM_HERE,
        base::Bind(running_ ?
                       &DebugCoreGdb::StepOver :
                       &DebugCoreGdb::RunToMain,
                   debug_core_));
    running_ = true;
    return true;
  } else if (key == kF11 && down && modifiers.None()) {
    AppThread::PostTask(AppThread::BACKEND, FROM_HERE,
        base::Bind(running_ ?
                       &DebugCoreGdb::StepIn :
                       &DebugCoreGdb::RunToMain,
                   debug_core_));
    running_ = true;
    return true;
  } else if (key == kF11 && down && modifiers.ShiftPressed()) {
    AppThread::PostTask(AppThread::BACKEND, FROM_HERE,
        base::Bind(&DebugCoreGdb::StepOut, debug_core_));
    return true;
  } else if (key == kF5 && down && modifiers.None()) {
    AppThread::PostTask(AppThread::BACKEND, FROM_HERE,
        base::Bind(&DebugCoreGdb::Continue, debug_core_));
    return true;
  } else if (key == kF5 && down &&
             modifiers.ShiftPressed() &&
             modifiers.ControlPressed()) {
    AppThread::PostTask(AppThread::BACKEND, FROM_HERE,
        base::Bind(&DebugCoreGdb::StopDebugging, debug_core_));
    AppThread::PostTask(AppThread::BACKEND, FROM_HERE,
        base::Bind(&DebugCoreGdb::RunToMain, debug_core_));
    return true;
  } else if (key == kF5 && down && modifiers.ShiftPressed()) {
    AppThread::PostTask(AppThread::BACKEND, FROM_HERE,
        base::Bind(&DebugCoreGdb::StopDebugging, debug_core_));
    return true;
  }
  return false;
}

void DebugPresenter::NotifyVariableExpansionStateChanged(
    const std::string& id, bool expanded) {
  if (!expanded) {
    int num_children = display_->GetLocalsChildCount(id);
    for (int i = num_children - 1; i >= 0; --i)
      display_->RemoveLocalsNode(display_->GetLocalsIdOfChild(id, i));
  }
  AppThread::PostTask(AppThread::BACKEND, FROM_HERE,
      base::Bind(&DebugCoreGdb::SetWatchExpanded, debug_core_, id, expanded));
}

void DebugPresenter::FileLoadCompleted(
    base::FilePath path, std::string* result) {
  // TODO(scottmg): mtime.
  source_files_->SetFileData(path, 0, *result);
  display_->SetFileName(path);
  display_->SetFileData(*result);
  delete result;
}

void DebugPresenter::SetDebugCore(base::WeakPtr<DebugCoreGdb> debug_core) {
  debug_core_ = debug_core;
  AppThread::PostTask(AppThread::BACKEND, FROM_HERE,
      base::Bind(&DebugCoreGdb::SetDebugNotification, debug_core_, this));
  AppThread::PostTask(AppThread::BACKEND, FROM_HERE,
      base::Bind(&DebugCoreGdb::LoadProcess,
                 debug_core, binary_, L"", std::vector<string16>(), L""));
}

void DebugPresenter::ReadFileOnFILE(base::FilePath path, std::string* result) {
  file_util::ReadFileToString(path, result);
}

void DebugPresenter::OnStoppedAtBreakpoint(
    const StoppedAtBreakpointData& data) {
  std::string* result = new std::string;
  // TODO(scottmg): Need to relativize to binary location (or search in some
  // reasonable way anyway).
  base::FilePath path(string16(L"test_data/") + data.frame.filename);
  AppThread::PostTaskAndReply(AppThread::FILE, FROM_HERE,
    base::Bind(&DebugPresenter::ReadFileOnFILE,
               base::Unretained(this), path, result),
    base::Bind(&DebugPresenter::FileLoadCompleted,
               base::Unretained(this), path, result));
  display_->SetProgramCounterLine(data.frame.line_number);
  UpdatePassiveDisplays();
}

void DebugPresenter::OnRetrievedStack(const RetrievedStackData& data) {
  // TODO(scottmg): Stack frame selection. Where should that live?
  display_->SetStackData(data.frames, 0);
}

// This should be moved to the debug core so that the ids can be created in a
// method that makes sense for it, but it's complex to do so asynchronously.
std::string DebugPresenter::GenerateNewVariableIdentifier() {
  return "V" + base::Int64ToString(variable_counter_++);
}

void DebugPresenter::OnRetrievedLocals(const RetrievedLocalsData& data) {
  std::map<std::string, string16> active_locals;

  // For each local, make sure we have a backend variable.
  for (size_t i = 0; i < data.local_names.size(); ++i) {
    const string16& local = data.local_names[i];
    std::map<string16, std::string>::const_iterator j;
    j = local_to_backend_.find(local);
    if (j == local_to_backend_.end()) {
      std::string id = GenerateNewVariableIdentifier();
      local_to_backend_[local] = id;
      display_->AddLocalsChild("", id);
      display_->SetLocalsNodeData(id, &local, NULL, NULL, NULL);
      AppThread::PostTask(AppThread::BACKEND, FROM_HERE,
          base::Bind(&DebugCoreGdb::CreateWatch,
                     debug_core_, id, local));
      active_locals[id] = local;
    } else {
      active_locals[j->second] = j->first;
    }
  }

  // Remove anything left in the view that's no longer active, |data| is always
  // a full set for this scope.
  std::vector<std::string> to_remove;
  for (int i = 0; i < display_->GetLocalsChildCount(""); ++i) {
    const std::string& child = display_->GetLocalsIdOfChild("", i);
    if (active_locals.find(child) == active_locals.end())
      to_remove.push_back(child);
  }

  for (size_t i = 0; i < to_remove.size(); ++i) {
    display_->RemoveLocalsNode(to_remove[i]);
    // TODO(scottmg): This search sucks.
    for (std::map<string16, std::string>::iterator j(local_to_backend_.begin());
         j != local_to_backend_.end(); ++j) {
      if (j->second == to_remove[i]) {
        local_to_backend_.erase(j->first);
        break;
      }
    }
    // TODO(scottmg): There's a race here. If we step again before this delete
    // completes, we'll get updates for variables we don't expect to exist, and
    // the locals view will dcheck. There could be a delete confirmation that
    // removes them from local_to_backend_, but then the case of creating a
    // variable while a delete is pending needs to be handled too. More
    // investigation and thought required.
    AppThread::PostTask(AppThread::BACKEND, FROM_HERE,
        base::Bind(&DebugCoreGdb::DeleteWatch,
                   debug_core_, to_remove[i]));
  }
}

void DebugPresenter::OnWatchCreated(const WatchCreatedData& data) {
  display_->SetLocalsNodeData(
      data.variable_id, NULL, &data.value, &data.type, &data.has_children);
}

void DebugPresenter::OnWatchesUpdated(const WatchesUpdatedData& data) {
  for (size_t i = 0; i < data.watches.size(); ++i) {
    const WatchesUpdatedData::Item& item = data.watches[i];
    // Has been deleted/removed.
    if (item.value == L"")
      continue;
    display_->SetLocalsNodeData(
        item.variable_id, NULL, &item.value, NULL, &item.has_children);
  }
}

void DebugPresenter::OnWatchChildList(const WatchesChildListData& data) {
  for (size_t i = 0; i < data.children.size(); ++i) {
    const WatchesChildListData::Child& child = data.children[i];
    display_->AddLocalsChild(data.parent, child.variable_id);
    display_->SetLocalsNodeData(
      child.variable_id,
      &child.expression,
      &child.value,
      &child.type,
      &child.has_children);
  }
}

void DebugPresenter::OnConsoleOutput(const string16& data) {
  display_->AddOutput(data);
}

void DebugPresenter::OnInternalDebugOutput(const string16& data) {
  display_->AddLog(data);
}

void DebugPresenter::OnStoppedAfterStepping(
    const StoppedAfterSteppingData& data) {
  // TODO(scottmg): File change reload, etc.
  display_->SetProgramCounterLine(data.frame.line_number);
  UpdatePassiveDisplays();
}

void DebugPresenter::OnLibraryLoaded(const LibraryLoadedData& data) {
  string16 output = L"Loaded '" + data.host_path + L"'";
  if (data.host_path != data.target_path)
    output += L" (" + data.target_path + L")";
  if (data.symbols_loaded)
    output += L", Symbols loaded.";
  display_->AddOutput(output);
}

void DebugPresenter::OnLibraryUnloaded(const LibraryUnloadedData& data) {
}

void DebugPresenter::UpdatePassiveDisplays() {
  AppThread::PostTask(AppThread::BACKEND, FROM_HERE,
      base::Bind(&DebugCoreGdb::GetStack, debug_core_));
  AppThread::PostTask(AppThread::BACKEND, FROM_HERE,
      base::Bind(&DebugCoreGdb::GetLocals, debug_core_));
  AppThread::PostTask(AppThread::BACKEND, FROM_HERE,
      base::Bind(&DebugCoreGdb::UpdateWatches, debug_core_));
}
