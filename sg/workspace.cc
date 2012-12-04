// Copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "sg/workspace.h"

#include "sg/application_window.h"
#include "sg/source_view.h"
#include "sg/status_bar.h"
#include "sg/ui/container.h"
#include "sg/ui/focus.h"
#include "sg/ui/solid_color.h"

namespace {

Container* Placeholder(const Skin& skin, const string16& name) {
  Container* container = new Container(skin);
  container->AddChild(new SolidColor(skin, skin.GetColorScheme().background()),
                      name);
  return container;
}

}  // namespace

Workspace::Workspace()
    : Container(this->skin_),
      delegate_(NULL) {
  main_area_ = new Container(skin_);
  status_bar_ = new StatusBar(skin_);
  AddChild(main_area_);
  AddChild(status_bar_);

  Container* top = new Container(skin_);
  Container* bottom = new Container(skin_);
  main_area_->SetMode(Container::SplitVertical);
  main_area_->Container::AddChild(top);
  main_area_->Container::AddChild(bottom);
  main_area_->SetFraction(top, .7);

  Contents* output = Placeholder(skin_, L"Output");
  Contents* log = Placeholder(skin_, L"Log");
  bottom->AddChild(output);
  bottom->AddChild(log);
  bottom->SetFraction(output, .6);

  source_view_ = new SourceView(skin_);
  source_view_container_ = new Container(skin_);
  source_view_container_->AddChild(source_view_);
  Container* middle = new Container(skin_);
  middle->SetMode(Container::SplitVertical);
  Container* views = new Container(skin_);
  views->SetMode(Container::SplitVertical);
  top->AddChild(source_view_container_);
  top->AddChild(middle);
  top->AddChild(views);
  top->SetFraction(source_view_container_, .4);
  top->SetFraction(middle, .65);

  Contents* stack = Placeholder(skin_, L"Stack");
  Contents* breakpoints = Placeholder(skin_, L"Breakpoints");
  middle->AddChild(stack);
  middle->AddChild(breakpoints);
  middle->SetFraction(stack, .75);

  Contents* watch = Placeholder(skin_, L"Watch");
  Contents* locals = Placeholder(skin_, L"Locals");
  views->AddChild(watch);
  views->AddChild(locals);
  views->SetFraction(watch, .6);

  SetFocusedContents(source_view_container_->Child(0));
}

Workspace::~Workspace() {
}

void Workspace::SetDelegate(ApplicationWindow* delegate) {
  delegate_ = delegate;
}

void Workspace::SetScreenRect(const Rect& rect) {
  set_screen_rect(rect);
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
}

void Workspace::Invalidate() {
  if (delegate_)
    delegate_->Paint();
}

void Workspace::SetFileName(const FilePath& filename) {
  source_view_container_->SetTitle(source_view_, filename.LossyDisplayName());
}

void Workspace::SetFileData(const std::string& utf8_text) {
  source_view_->SetData(utf8_text);
}

void Workspace::SetDebugState(const string16& debug_state) {
  status_bar_->SetDebugState(debug_state);
}

void Workspace::SetRenderTime(double frame_time_in_ms) {
  status_bar_->SetRenderTime(frame_time_in_ms);
}
