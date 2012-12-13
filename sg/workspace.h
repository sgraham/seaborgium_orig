// Copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef SG_WORKSPACE_H_
#define SG_WORKSPACE_H_

#include <string>
#include <vector>

#include "sg/debug_presenter_display.h"
#include "sg/ui/container.h"
#include "sg/ui/input.h"
#include "sg/ui/skin.h"

class ApplicationWindow;
class Container;
class DebugPresenterNotify;
class SourceView;
class StackView;
class StatusBar;

class Workspace : public Container,
                  public DebugPresenterDisplay {
 public:
  Workspace();
  virtual ~Workspace();

  virtual void Init();
  virtual void SetDelegate(ApplicationWindow* delegate);
  virtual void SetDebugPresenterNotify(DebugPresenterNotify* debug_presenter);

  virtual void SetScreenRect(const Rect& rect) OVERRIDE;
  virtual void Invalidate() OVERRIDE;

  // Implementation of InputHandler.
  virtual bool NotifyMouseMoved(
      int x, int y, int dx, int dy, const InputModifiers& modifiers) OVERRIDE;
  virtual bool NotifyMouseWheel(
      int delta, const InputModifiers& modifiers) OVERRIDE;
  virtual bool NotifyKey(
      InputKey key, bool down, const InputModifiers& modifiers) OVERRIDE;
  virtual bool WantMouseEvents() OVERRIDE { return true; }
  virtual bool WantKeyEvents() OVERRIDE { return true; }

  // Implementation of DebugPresenterDisplay.
  virtual void SetFileName(const FilePath& filename) OVERRIDE;
  virtual void SetFileData(const std::string& utf8_text) OVERRIDE;
  virtual void SetProgramCounterLine(int line_number) OVERRIDE;
  virtual void SetStackData(
      const std::vector<FrameData>& frame_data, int active) OVERRIDE;
  virtual void SetDebugState(const string16& debug_state) OVERRIDE;
  virtual void SetRenderTime(double ms_per_frame) OVERRIDE;

 private:
  Skin skin_;

  Container* main_area_;
  StatusBar* status_bar_;

  SourceView* source_view_;
  Container* source_view_container_;
  StackView* stack_view_;
  Container* stack_view_container_;
  ApplicationWindow* delegate_;
  DebugPresenterNotify* debug_presenter_notify_;
};

#endif  // SG_WORKSPACE_H_
