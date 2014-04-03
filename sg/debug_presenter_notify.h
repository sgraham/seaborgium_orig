// Copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef SG_DEBUG_PRESENTER_NOTIFY_H_
#define SG_DEBUG_PRESENTER_NOTIFY_H_

#include <string>

#include "sg/base/string16.h"
#include "sg/ui/input.h"

// Notification from view back up to presenter.
class DebugPresenterNotify {
 public:
  virtual ~DebugPresenterNotify() {}
  virtual void NotifyFramePainted(double frame_time_in_ms) = 0;
  virtual bool NotifyKey(
      InputKey key, bool down, const InputModifiers& modifiers) = 0;
  virtual void NotifyVariableExpansionStateChanged(
      const std::string& id, bool expanded) = 0;
};

#endif  // SG_DEBUG_PRESENTER_NOTIFY_H_
