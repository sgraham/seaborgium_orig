// Copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "sg/top_level_frame.h"

#include "Gwen/Controls/Layout/Position.h"
#include "Gwen/Controls/ListBox.h"
#include "Gwen/Controls/TabControl.h"
#include "Gwen/Controls/WindowControl.h"
#include "Gwen/Platform.h"
#include "sg/workspace.h"

GWEN_CONTROL_CONSTRUCTOR(TopLevelFrame) {
  Dock(Gwen::Pos::Fill);

  workspace_ = new Workspace(this);

  /*
  status_bar_ = new Controls::StatusBar(this);
  status_bar_->Dock(Pos::Bottom);

  main_frame_ = new MainFrame(this);
  main_frame_->Dock(Pos::Fill);
  status_bar_->SendToBack();

  frame_sum_in_seconds_ = 0.0;
  last_time_ = Gwen::Platform::GetTimeInSeconds();
  frames_ = 0;
  */
}

void TopLevelFrame::Render(Gwen::Skin::Base* skin) {
  /*
  frames_++;
  double time = Gwen::Platform::GetTimeInSeconds();
  frame_sum_in_seconds_ += time - last_time_;
  last_time_ = time;
  if (frames_ == 10) {
    status_bar_->SetText(Gwen::Utility::Format(L"%.1f ms",
        (frame_sum_in_seconds_ * 1000.0) / frames_));
    frames_ = 0;
    frame_sum_in_seconds_ = 0;
  }
*/
  workspace_->Render(skin);
  BaseClass::Render(skin);
}
