// Copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef SG_TOP_LEVEL_FRAME_H_
#define SG_TOP_LEVEL_FRAME_H_

#include "Gwen/Gwen.h"
#include "Gwen/Align.h"
#include "Gwen/Utility.h"
#include "Gwen/Controls/DockBase.h"
#include "Gwen/Controls/StatusBar.h"

namespace Gwen { namespace Controls { class ListBox; } }
class MainFrame;
class Workspace;

// Contains a status bar and a docking container
class TopLevelFrame : public Gwen::Controls::Base {
 public:
  GWEN_CONTROL(TopLevelFrame, Gwen::Controls::Base);
  void Render(Gwen::Skin::Base* skin);

 private:
  MainFrame* main_frame_;
  Gwen::Controls::StatusBar* status_bar_;
  unsigned int frames_;
  double frame_sum_in_seconds_;
  double last_time_;
  Workspace* workspace_;
};

#endif  // SG_TOP_LEVEL_FRAME_H_
