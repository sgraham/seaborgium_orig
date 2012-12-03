// Copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "sg/workspace.h"

#include "ui/container.h"
#include "ui/focus.h"
#include "ui/solid_color.h"
#include "sg/source_view.h"

namespace {

Container* Placeholder(const Skin& skin, const string16& name) {
  Container* container = new Container(skin);
  container->AddChild(new SolidColor(skin, skin.GetColorScheme().background()),
                      name);
  return container;
}

Container* MakeSourceView(const Skin& skin,
                          const string16& source_file) {
  Container* container = new Container(skin);
  SourceView* source_view = new SourceView(skin);
  container->AddChild(source_view, source_file);
  return container;
}

}  // namespace

Workspace::Workspace()
    : Container(this->skin_) {
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

  Container* source = MakeSourceView(skin_, L"sample_source_code_file.cc");
  Container* middle = new Container(skin_);
  middle->SetMode(Container::SplitVertical);
  Container* views = new Container(skin_);
  views->SetMode(Container::SplitVertical);
  top->AddChild(source);
  top->AddChild(middle);
  top->AddChild(views);
  top->SetFraction(source, .4);
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

  SetFocusedContents(source->Child(0));
}

Workspace::~Workspace() {
}
