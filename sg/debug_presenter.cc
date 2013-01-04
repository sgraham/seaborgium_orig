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
  // TODO(scottmg): Relative to gdb location.
  if (command_line.GetArgs().size() != 1)
    binary_ = L"../test_data/test_binary_mingw.exe";
  else
    ReplaceChars(command_line.GetArgs()[0], L"\\", L"/", &binary_);
}

DebugPresenter::~DebugPresenter() {
}

void DebugPresenter::SetDisplay(DebugPresenterDisplay* display) {
  display_ = display;
  if (!running_)
    display_->SetFileData("\n"
                          "Binary from command line (or default test) loaded.\n"
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

void DebugPresenter::ReadFileOnFILE(FilePath path, std::string* result) {
  file_util::ReadFileToString(path, result);
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
      base::Bind(&DebugCoreGdb::SetDebugNotification, debug_core_, this));
  AppThread::PostTask(AppThread::BACKEND, FROM_HERE,
      base::Bind(&DebugCoreGdb::LoadProcess,
                 debug_core, binary_, L"", std::vector<string16>(), L""));
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
  typedef std::map<string16, DebugPresenterVariable> KeyToVariableMap;

  // Make |DebugPresenterVariable|s for all the ones we want to display now.
  KeyToVariableMap new_locals;
  for (size_t i = 0; i < data.locals.size(); ++i) {
    DebugPresenterVariable new_variable(
        data.locals[i].type, data.locals[i].name);
    new_locals[new_variable.key()] = new_variable;
  }

  // Get all the locals currently in the view, mapped by key.
  KeyToVariableMap in_view;
  typedef std::map<string16, int> KeyToIndexMap;
  KeyToIndexMap view_indices;
  for (int i = 0; i < display_->NumLocals(); ++i) {
    const DebugPresenterVariable& variable = display_->GetLocal(i);
    in_view[variable.key()] = variable;
    view_indices[variable.key()] = i;
  }

  for (KeyToVariableMap::iterator i(new_locals.begin());
       i != new_locals.end(); ++i) {
    // If we already have it, then don't do anything (to preserve any existing
    // backend variable), otherwise append the new variable.
    if (in_view.find(i->first) == in_view.end()) {
      std::string id = GenerateNewVariableIdentifier();
      i->second.set_backend_id(id);
      display_->SetLocal(display_->NumLocals(), i->second);
      AppThread::PostTask(AppThread::BACKEND, FROM_HERE,
          base::Bind(&DebugCoreGdb::CreateWatch,
                     debug_core_, id, i->second.name()));
    }
  }

  // We've now added all the locals we want to exist, but not removed ones
  // that have gone out of scope. Walk the view, and remove any that aren't in
  // new_locals.
  std::vector<int> indices_to_remove;
  for (KeyToVariableMap::const_iterator i(in_view.begin());
       i != in_view.end(); ++i) {
    if (new_locals.find(i->first) == new_locals.end())
      indices_to_remove.push_back(view_indices[i->first]);
  }
  std::sort(indices_to_remove.begin(), indices_to_remove.end());
  std::reverse(indices_to_remove.begin(), indices_to_remove.end());
  for (size_t i = 0; i < indices_to_remove.size(); ++i)
    display_->RemoveLocal(indices_to_remove[i]);
}

void DebugPresenter::OnWatchCreated(const WatchCreatedData& data) {
  display_->SetLocalValue(data.variable_id, data.value);
  display_->SetLocalHasChildren(data.variable_id, data.has_children);
}

void DebugPresenter::OnWatchesUpdated(const WatchesUpdatedData& data) {
  for (size_t i = 0; i < data.watches.size(); ++i) {
    const WatchesUpdatedData::Item& item = data.watches[i];
    display_->SetLocalValue(item.variable_id, item.value);
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
