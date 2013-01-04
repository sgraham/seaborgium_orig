// Copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef SG_WORKSPACE_H_
#define SG_WORKSPACE_H_

#include <string>
#include <vector>

#include "sg/debug_presenter_display.h"
#include "sg/ui/docking_workspace.h"
#include "sg/ui/input.h"
#include "sg/ui/skin.h"

class ApplicationWindow;
class DebugPresenterNotify;
class DockingResizer;
class LocalsView;
class ScrollingOutputView;
class SourceView;
class StackView;
class StatusBar;

class Workspace : public DebugPresenterDisplay, public InputHandler {
 public:
  Workspace();
  virtual ~Workspace();

  virtual void Init();
  virtual void SetDelegate(ApplicationWindow* delegate);
  virtual void SetDebugPresenterNotify(DebugPresenterNotify* debug_presenter);

  virtual void SetScreenRect(const Rect& rect);
  static void Invalidate();

  virtual void Render(Renderer* renderer);

  // Implementation of InputHandler.
  virtual bool NotifyMouseMoved(
      int x, int y, int dx, int dy, const InputModifiers& modifiers) OVERRIDE;
  virtual bool NotifyMouseWheel(
      int delta, const InputModifiers& modifiers) OVERRIDE;
  virtual bool NotifyMouseButton(
      int index, bool down, const InputModifiers& modifiers) OVERRIDE;
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
  virtual int NumLocals() OVERRIDE;
  virtual DebugPresenterVariable GetLocal(int local) OVERRIDE;
  virtual void SetLocal(
      int local, const DebugPresenterVariable& variable) OVERRIDE;
  virtual void RemoveLocal(int local) OVERRIDE;
  virtual void SetLocalValue(
    const std::string& id, const string16& value) OVERRIDE;
  virtual void SetLocalHasChildren(
    const std::string& id, bool has_children) OVERRIDE;
  virtual void AddOutput(const string16& text) OVERRIDE;
  virtual void AddLog(const string16& text) OVERRIDE;
  virtual void SetRenderTime(double ms_per_frame) OVERRIDE;

 private:
  void InvalidateImpl();

  scoped_ptr<DockingWorkspace> main_area_;
  StatusBar* status_bar_;

  SourceView* source_view_;
  StackView* stack_view_;
  Dockable* stack_view_window_;
  ScrollingOutputView* output_;
  Dockable* output_window_;
  ScrollingOutputView* log_;
  Dockable* log_window_;
  LocalsView* locals_view_;
  Dockable* locals_view_window_;

  Dockable* watch_;
  Dockable* breakpoints_;

  ApplicationWindow* delegate_;
  DebugPresenterNotify* debug_presenter_notify_;

  scoped_ptr<Draggable> draggable_;

  Point mouse_position_;
};

#endif  // SG_WORKSPACE_H_
