// Copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "sg/ui/focus.h"

namespace {

Contents* g_focused;
Contents* g_hovered;

}  // namespace

// TODO(focus): Probably some sort of OnFocus/OnBlur?

Contents* GetFocusedContents() {
  return g_focused;
}

void SetFocusedContents(Contents* contents) {
  g_focused = contents;
}

Contents* GetHoveredContents() {
  return g_hovered;
}

void SetHoveredContents(Contents* contents) {
  g_hovered = contents;
}
