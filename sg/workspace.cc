// Copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "sg/workspace.h"

#include "base/bind.h"
#include "sg/app_thread.h"
#include "sg/debug_presenter_notify.h"
#include "sg/render/application_window.h"
#include "sg/source_view.h"
#include "sg/stack_view.h"
#include "sg/status_bar.h"
#include "sg/ui/focus.h"
#include "sg/ui/solid_color.h"

namespace {

  /*
Container* Placeholder(const Skin& skin, const string16& name) {
  Container* container = new Container(skin);
  container->AddChild(new SolidColor(skin, skin.GetColorScheme().background()),
                      name);
  return container;
}
*/

}  // namespace

Workspace::Workspace()
    : delegate_(NULL),
      main_area_(NULL),
      status_bar_(NULL) /*,
      source_view_(NULL),
      source_view_container_(NULL)*/ {
  // Initialization deferred until Init when we know our window size.
}

void Workspace::Init() {
  DCHECK(!main_area_);
  /*
  main_area_ = new Container(skin_);
  status_bar_ = new StatusBar(skin_);
  AddChild(main_area_);
  AddChild(status_bar_);

  source_view_ = new SourceView(skin_);
  source_view_container_ = new Container(skin_);
  source_view_container_->AddChild(source_view_);

  stack_view_ = new StackView(skin_);
  stack_view_container_ = new Container(skin_);
  stack_view_container_->AddChild(stack_view_, L"Stack");

  Contents* output = Placeholder(skin_, L"Output");
  Contents* log = Placeholder(skin_, L"Log");
  Contents* watch = Placeholder(skin_, L"Watch");
  Contents* locals = Placeholder(skin_, L"Locals");
  Contents* breakpoints = Placeholder(skin_, L"Breakpoints");

  bool is_landscape = GetClientRect().w > GetClientRect().h;
  if (is_landscape) {
    Container* top = new Container(skin_);
    Container* bottom = new Container(skin_);
    main_area_->SetMode(Container::SplitVertical);
    main_area_->Container::AddChild(top);
    main_area_->Container::AddChild(bottom);
    main_area_->SetFraction(top, .7);

    bottom->AddChild(output);
    bottom->AddChild(log);
    bottom->SetFraction(output, .6);

    Container* middle = new Container(skin_);
    middle->SetMode(Container::SplitVertical);
    Container* views = new Container(skin_);
    views->SetMode(Container::SplitVertical);
    top->AddChild(source_view_container_);
    top->AddChild(middle);
    top->AddChild(views);
    top->SetFraction(source_view_container_, .375);
    top->SetFraction(middle, .65);

    middle->AddChild(stack_view_container_);
    middle->AddChild(breakpoints);
    middle->SetFraction(stack_view_container_, .75);

    views->AddChild(watch);
    views->AddChild(locals);
    views->SetFraction(watch, .6);
  } else {
    Container* first_row = new Container(skin_);
    Container* second_row = new Container(skin_);
    Container* third_row = new Container(skin_);
    main_area_->SetMode(Container::SplitVertical);
    main_area_->AddChild(first_row);
    main_area_->AddChild(second_row);
    main_area_->AddChild(third_row);
    main_area_->SetFraction(first_row, .5);
    main_area_->SetFraction(second_row, .8);

    Container* top_right = new Container(skin_);
    top_right->SetMode(Container::SplitVertical);
    top_right->AddChild(stack_view_container_);
    top_right->AddChild(breakpoints);
    first_row->AddChild(source_view_container_);
    first_row->AddChild(top_right);
    first_row->SetFraction(source_view_container_, .55);

    Container* views = new Container(skin_);
    views->SetMode(Container::SplitHorizontal);
    second_row->AddChild(watch);
    second_row->AddChild(locals);
    second_row->SetFraction(watch, .6);

    third_row->AddChild(output);
    third_row->AddChild(log);
    third_row->SetFraction(output, .7);
  }

  SetFocusedContents(source_view_container_->Child(0));
  */
}

Workspace::~Workspace() {
}

void Workspace::SetDelegate(ApplicationWindow* delegate) {
  delegate_ = delegate;
}

void Workspace::SetDebugPresenterNotify(DebugPresenterNotify* debug_presenter) {
  debug_presenter_notify_ = debug_presenter;
}

void Workspace::SetScreenRect(const Rect& rect) {
  /*
  Contents::SetScreenRect(rect);
  if (!main_area_)
    Init();
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
  if (delegate_) {
    AppThread::PostDelayedTask(AppThread::UI, FROM_HERE,
        base::Bind(&ApplicationWindow::Paint, base::Unretained(delegate_)),
        base::TimeDelta::FromMilliseconds(10));
  }
}

void Workspace::Render(Renderer* renderer) {
}

void Workspace::SetFileName(const FilePath& filename) {
  /*source_view_container_->SetTitle(source_view_, filename.LossyDisplayName());
   */
}

void Workspace::SetProgramCounterLine(int line_number) {
  /*
  source_view_->SetProgramCounterLine(line_number);
  */
}

void Workspace::SetFileData(const std::string& utf8_text) {
  /*
  source_view_->SetData(utf8_text);
  */
}

void Workspace::SetDebugState(const string16& debug_state) {
  status_bar_->SetDebugState(debug_state);
}

void Workspace::SetRenderTime(double frame_time_in_ms) {
  status_bar_->SetRenderTime(frame_time_in_ms);
}

void Workspace::SetStackData(
    const std::vector<FrameData>& frame_data, int active) {
  /*
  stack_view_->SetData(frame_data, active);
  */
}

void Workspace::SetLocalsData(const std::vector<TypeNameValue>& locals_data) {
  // locals_view_->SetData(locals_data);
}

bool Workspace::NotifyMouseMoved(
    int x, int y, int dx, int dy, const InputModifiers& modifiers) {
  /*
  mouse_position_.x = x;
  mouse_position_.y = y;
  UpdateHovered();
  Contents* focused = GetFocusedContents();
  if (!focused || !focused->WantMouseEvents())
    return false;
  return focused->NotifyMouseMoved(x, y, dx, dy, modifiers);
  */
  return false;
}

bool Workspace::NotifyMouseWheel(int delta, const InputModifiers& modifiers) {
  /*
  Contents* focused = GetFocusedContents();
  if (!focused || !focused->WantMouseEvents())
    return false;
  return focused->NotifyMouseWheel(delta, modifiers);
  */
  return false;
}

bool Workspace::NotifyMouseButton(
    int index, bool down, const InputModifiers& modifiers) {
  // TODO(input): Something! Change focus, forward, etc.
  return false;
}

bool Workspace::NotifyKey(
    InputKey key, bool down, const InputModifiers& modifiers) {
  /*
  Contents* focused = GetFocusedContents();
  if (!focused || !focused->WantKeyEvents())
    return false;
  if (focused->NotifyKey(key, down, modifiers))
    return true;
  return debug_presenter_notify_->NotifyKey(key, down, modifiers);
  */
  return false;
}

void Workspace::UpdateHovered() {
  /*
  // TODO(scottmg): Dispatch a mouse-leave and a mouse-enter appropriately.
  SetHoveredContents(FindContentsAt(mouse_position_));
  Invalidate();
  */
}
