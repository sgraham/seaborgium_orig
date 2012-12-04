// Copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "sg/application.h"

#include <vector>

#include "base/logging.h"
#include "sg/application_window.h"
#include "sg/backend/backend_native_win.h"
#include "sg/debug_presenter.h"
#include "sg/source_files.h"
#include "sg/workspace.h"

Application::Application() {
  source_files_.reset(new SourceFiles);
  main_window_.reset(ApplicationWindow::Create());
  workspace_.reset(new Workspace);
  main_window_->SetContents(workspace_.get());
  workspace_->SetDelegate(main_window_.get());
  presenter_.reset(new DebugPresenter(source_files_.get()));
  presenter_->SetDisplay(workspace_.get());
  main_window_->SetDebugPresenterNotify(presenter_.get());
  debug_connection_.reset(new DebugConnectionNativeWin(presenter_.get()));
  std::vector<string16> empty_environment;
  string16 error;
  Process* p = debug_connection_->ProcessCreate(
      L"test_binary.exe",
      L"this is stuff",
      empty_environment,
      L"",
      &error);
  DVLOG(1) << p;
}

Application::~Application() {
}
