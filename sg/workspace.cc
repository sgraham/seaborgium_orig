// Copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "sg/workspace.h"

#include "base/bind.h"
#include "sg/app_thread.h"
#include "sg/debug_presenter_notify.h"
#include "sg/locals_view.h"
#include "sg/render/application_window.h"
#include "sg/source_view.h"
#include "sg/stack_view.h"
#include "sg/status_bar.h"
#include "sg/ui/docking_split_container.h"
#include "sg/ui/docking_resizer.h"
#include "sg/ui/docking_tool_window.h"
#include "sg/ui/focus.h"
#include "sg/ui/scrolling_output_view.h"
#include "sg/ui/skin.h"
#include "sg/ui/solid_color.h"

namespace {

Workspace* g_main_workspace;

Dockable* Placeholder(const string16& name) {
  return new DockingToolWindow(
      new SolidColor(Skin::current().GetColorScheme().background()), name);
}

}  // namespace

Workspace::Workspace()
    : delegate_(NULL),
      status_bar_(NULL) {
  // Initialization deferred until Init when we know our window size.
  // Objects created now so they're ready to receive data early in startup.
  source_view_ = new SourceView;
  stack_view_ = new StackView;
  stack_view_window_ = new DockingToolWindow(stack_view_, L"Stack");
  watch_ = Placeholder(L"Watch");
  locals_view_ = new LocalsView;
  locals_view_window_ = new DockingToolWindow(locals_view_, L"Locals");
  breakpoints_ = Placeholder(L"Breakpoints");
  output_ = new ScrollingOutputView;
  output_window_ = new DockingToolWindow(output_, L"Output");
  log_ = new ScrollingOutputView;
  log_window_ = new DockingToolWindow(log_, L"Log");

  // For Invalidate, and interaction with dragger. Could use some sort of
  // broadcast for Invalidate (and coalesce there) and could pass to dragger.
  CHECK(!g_main_workspace);
  g_main_workspace = this;
}

Workspace::~Workspace() {
  CHECK(g_main_workspace == this);
  g_main_workspace = NULL;
}

void Workspace::Init() {
  DCHECK(!main_area_);
  main_area_.reset(new DockingWorkspace);
  main_area_->SetRoot(source_view_);

  if (delegate_->IsLandscape()) {
    source_view_->parent()->SplitChild(
        kSplitHorizontal, source_view_, output_window_);
    source_view_->parent()->SetFraction(.7);
    output_window_->parent()->SplitChild(
        kSplitVertical, output_window_, log_window_);
    output_window_->parent()->SetFraction(.6);

    source_view_->parent()->SplitChild(
        kSplitVertical, source_view_, stack_view_window_);
    source_view_->parent()->SetFraction(.375);
    stack_view_window_->parent()->SplitChild(
        kSplitVertical, stack_view_window_, watch_);
    stack_view_window_->parent()->SetFraction(.5);
    watch_->parent()->SplitChild(kSplitHorizontal, watch_, locals_view_window_);
    watch_->parent()->SetFraction(.65);
    stack_view_window_->parent()->SplitChild(
        kSplitHorizontal, stack_view_window_, breakpoints_);
    stack_view_window_->parent()->SetFraction(.6);
  } else {
    source_view_->parent()->SplitChild(
        kSplitHorizontal, source_view_, output_window_);
    source_view_->parent()->SetFraction(.75);
    output_window_->parent()->SplitChild(
        kSplitVertical, output_window_, log_window_);
    output_window_->parent()->SetFraction(.6);
    log_window_->parent()->SplitChild(
        kSplitHorizontal, breakpoints_, log_window_);

    source_view_->parent()->SplitChild(kSplitVertical, source_view_, watch_);
    watch_->parent()->SplitChild(kSplitHorizontal, watch_, locals_view_window_);
    locals_view_window_->parent()->SplitChild(
        kSplitHorizontal, locals_view_window_, stack_view_window_);
  }

  SetFocusedContents(source_view_);
}

void Workspace::SetDelegate(ApplicationWindow* delegate) {
  delegate_ = delegate;
}

void Workspace::SetDebugPresenterNotify(DebugPresenterNotify* debug_presenter) {
  debug_presenter_notify_ = debug_presenter;
}

void Workspace::SetScreenRect(const Rect& rect) {
  if (!main_area_)
    Init();
  // TODO(scottmg): Space for status bar, etc.
  main_area_->SetScreenRect(rect);
  /*
  const Skin& skin = GetSkin();
  Rect outer_border_size(
      skin.border_size(), skin.border_size(),
      skin.border_size(), skin.border_size());
  Rect remaining = rect.Contract(outer_border_size);
  Rect main_area_rect = remaining.Contract(
      Rect(0, 0, 0, skin.status_bar_size()));
  main_area_->SetScreenRect(main_area_rect);
  status_bar_->SetScreenRect(
      Rect(remaining.x, remaining.y + main_area_rect.h + skin.border_size(),
           remaining.w, skin.status_bar_size() - skin.border_size()));
           */
}

void Workspace::Invalidate() {
  g_main_workspace->InvalidateImpl();
}

void Workspace::InvalidateImpl() {
  if (delegate_) {
    AppThread::PostDelayedTask(AppThread::UI, FROM_HERE,
        base::Bind(&ApplicationWindow::Paint, base::Unretained(delegate_)),
        base::TimeDelta::FromMilliseconds(10));
  }
}

void Workspace::Render(Renderer* renderer) {
  Skin::EnsureTexturesLoaded(renderer);
  main_area_->Render(renderer);
  if (draggable_.get())
    draggable_->Render(renderer);
}

void Workspace::SetFileName(const FilePath& filename) {
  /*source_view_container_->SetTitle(source_view_, filename.LossyDisplayName());
   */
}

void Workspace::SetProgramCounterLine(int line_number) {
  source_view_->SetProgramCounterLine(line_number);
}

void Workspace::SetFileData(const std::string& utf8_text) {
  source_view_->SetData(utf8_text);
}

void Workspace::SetRenderTime(double frame_time_in_ms) {
  status_bar_->SetRenderTime(frame_time_in_ms);
}

void Workspace::SetStackData(
    const std::vector<FrameData>& frame_data, int active) {
  stack_view_->SetData(frame_data, active);
}

int Workspace::NumLocals() {
  return locals_view_->NumLocals();
}

DebugPresenterVariable Workspace::GetLocal(int local) {
  return locals_view_->GetLocal(local);
}

void Workspace::SetLocal(int local, const DebugPresenterVariable& variable) {
  return locals_view_->SetLocal(local, variable);
}

void Workspace::RemoveLocal(int local) {
  return locals_view_->RemoveLocal(local);
}

void Workspace::SetLocalValue(const std::string& id, const string16& value) {
  return locals_view_->SetLocalValue(id, value);
}

void Workspace::SetLocalHasChildren(const std::string& id, bool has_children) {
  return locals_view_->SetLocalHasChildren(id, has_children);
}

void Workspace::AddOutput(const string16& text) {
  output_->AddText(text);
}

void Workspace::AddLog(const string16& text) {
  log_->AddText(text);
}

bool Workspace::NotifyMouseMoved(
    int x, int y, int dx, int dy, const InputModifiers& modifiers) {
  mouse_position_.x = x;
  mouse_position_.y = y;
  if (draggable_.get()) {
    draggable_->Drag(mouse_position_);
    InvalidateImpl();
    return true;
  }
  DragSetup drag_setup(mouse_position_, main_area_.get());
  if (main_area_->CouldStartDrag(&drag_setup)) {
    if (drag_setup.drag_direction == kDragDirectionLeftRight)
      SetCursor(LoadCursor(NULL, IDC_SIZEWE));
    else if (drag_setup.drag_direction == kDragDirectionUpDown)
      SetCursor(LoadCursor(NULL, IDC_SIZENS));
    else if (drag_setup.drag_direction == kDragDirectionAll)
      SetCursor(LoadCursor(NULL, IDC_SIZEALL));
  } else {
    SetCursor(LoadCursor(NULL, IDC_ARROW));
  }
  Dockable* focused = GetFocusedContents();
  if (!focused || !focused->WantMouseEvents())
    return false;
  return focused->NotifyMouseMoved(x, y, dx, dy, modifiers);
}

bool Workspace::NotifyMouseWheel(int delta, const InputModifiers& modifiers) {
  Dockable* focused = GetFocusedContents();
  if (!focused || !focused->WantMouseEvents())
    return false;
  return focused->NotifyMouseWheel(delta, modifiers);
}

bool Workspace::NotifyMouseButton(
    int index, bool down, const InputModifiers& modifiers) {
  DragSetup drag_setup(mouse_position_, main_area_.get());
  drag_setup.draggable = &draggable_;
  if (draggable_.get() && index == 0 && !down) {
    draggable_.reset();
    SetCursor(LoadCursor(NULL, IDC_ARROW));
    Invalidate();
    return true;
  } else if (index == 0 && down && main_area_->CouldStartDrag(&drag_setup)) {
    Invalidate();
    return true;
  } else if (index == 0 && down) {
    Dockable* target = main_area_->FindTopMostUnderPoint(mouse_position_);
    SetFocusedContents(target);
    target->NotifyMouseMoved(mouse_position_.x, mouse_position_.y,
                             0, 0, modifiers);
    target->NotifyMouseButton(index, down, modifiers);
    Invalidate();
  }
  return false;
}

bool Workspace::NotifyKey(
    InputKey key, bool down, const InputModifiers& modifiers) {
  Dockable* focused = GetFocusedContents();
  if (!focused)
    return false;
  if (focused->WantKeyEvents() && focused->NotifyKey(key, down, modifiers))
    return true;
  return debug_presenter_notify_->NotifyKey(key, down, modifiers);
}
