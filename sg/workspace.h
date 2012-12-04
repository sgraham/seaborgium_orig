// Copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef SG_WORKSPACE_H_
#define SG_WORKSPACE_H_

#include <string>

#include "sg/debug_presenter_display.h"
#include "sg/debug_presenter_notify.h"
#include "sg/ui/container.h"
#include "sg/ui/skin.h"

class ApplicationWindow;
class Container;
class SourceView;
class StatusBar;

class Workspace : public Container,
                  public DebugPresenterDisplay,
                  public DebugPresenterNotify {
 public:
  Workspace();
  virtual ~Workspace();

  virtual void SetDelegate(ApplicationWindow* delegate);

  virtual void SetScreenRect(const Rect& rect) OVERRIDE;
  virtual void Invalidate() OVERRIDE;

  // Implementation of DebugPresenterDisplay.
  virtual void SetFileName(const FilePath& filename) OVERRIDE;
  virtual void SetFileData(const std::string& utf8_text) OVERRIDE;
  virtual void SetDebugState(const string16& debug_state);

  // Implementation of DebugPresenterDisplay.
  virtual void NotifyFramePainted(double frame_time_in_ms);

 private:
  Skin skin_;

  Container* main_area_;
  StatusBar* status_bar_;

  SourceView* source_view_;
  Container* source_view_container_;
  ApplicationWindow* delegate_;
};

#endif  // SG_WORKSPACE_H_
