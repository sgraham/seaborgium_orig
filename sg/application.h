// Copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef SG_APPLICATION_H_
#define SG_APPLICATION_H_

#include "base/basictypes.h"
#include "base/memory/scoped_ptr.h"

class ApplicationWindow;
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
  scoped_ptr<SourceFiles> source_files_;
  scoped_ptr<ApplicationWindow> main_window_;
  scoped_ptr<Workspace> workspace_;
  scoped_ptr<DebugPresenter> presenter_;

  DISALLOW_COPY_AND_ASSIGN(Application);
};

#endif  // SG_APPLICATION_H_
