// Copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef SG_APPLICATION_H_
#define SG_APPLICATION_H_

#include <memory>

#include "base/basictypes.h"
#include "base/memory/weak_ptr.h"
#include "sg/backend/backend.h"

class ApplicationWindow;
class DebugCoreGdb;
class DebugPresenter;
class SourceFiles;
class Workspace;

// Top level. Initializes system-level rendering, creates MVP hierarchy and
// hooks them together. Currently owns one ApplicationWindow, but will need to
// be multiple.
class Application {
 public:
  Application();
  virtual ~Application();

 private:
  std::unique_ptr<SourceFiles> source_files_;
  std::unique_ptr<ApplicationWindow> main_window_;
  std::unique_ptr<Workspace> workspace_;
  std::unique_ptr<DebugPresenter> presenter_;
  base::WeakPtr<DebugCoreGdb> debug_core_;

  void ConnectDebugCore(base::WeakPtr<DebugCoreGdb> debug_core);

  DISALLOW_COPY_AND_ASSIGN(Application);
};

#endif  // SG_APPLICATION_H_
