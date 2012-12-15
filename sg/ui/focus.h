// Copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef SG_UI_FOCUS_H_
#define SG_UI_FOCUS_H_

class Contents;

Contents* GetFocusedContents();
void SetFocusedContents(Contents* contents);

Contents* GetHoveredContents();
void SetHoveredContents(Contents* contents);

#endif  // SG_UI_FOCUS_H_
