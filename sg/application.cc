// Copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "sg/application.h"

#include <vector>

#include "base/logging.h"
#include "sg/app_thread.h"
#include "sg/backend/debug_core_gdb.h"
#include "sg/debug_presenter.h"
#include "sg/render/application_window.h"
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
  workspace_->SetDebugPresenterNotify(presenter_.get());
  AppThread::PostTaskAndReplyWithResult(
      AppThread::BACKEND, FROM_HERE,
      base::Bind(&DebugCoreGdb::Create),
      base::Bind(&Application::ConnectDebugCore, base::Unretained(this)));
}

Application::~Application() {
}

void Application::ConnectDebugCore(base::WeakPtr<DebugCoreGdb> debug_core) {
  presenter_->SetDebugCore(debug_core);
}
