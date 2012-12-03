// Copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "sg/workspace.h"

#include "sg/application_window.h"
#include "sg/source_view.h"
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
  Container* top = new Container(skin_);
  Container* bottom = new Container(skin_);
  this->SetMode(Container::SplitVertical);
  this->Container::AddChild(top);
  this->Container::AddChild(bottom);
  this->SetFraction(top, .7);

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
