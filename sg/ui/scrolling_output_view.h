// Copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef SG_UI_SCROLLING_OUTPUT_VIEW_H_
#define SG_UI_SCROLLING_OUTPUT_VIEW_H_

#include "base/string16.h"
#include "sg/ui/dockable.h"
#include "sg/ui/scroll_helper.h"

class ScrollingOutputView : public Dockable, public ScrollHelperDataProvider {
 public:
  ScrollingOutputView();
  virtual ~ScrollingOutputView();

  virtual void Render(Renderer* renderer) OVERRIDE;

  virtual void AddText(const string16& text);

  // Implementation of InputHandler:
  virtual bool NotifyMouseMoved(
      int x, int y, int dx, int dy, const InputModifiers& modifiers) OVERRIDE;
  virtual bool NotifyMouseWheel(
      int delta, const InputModifiers& modifiers) OVERRIDE;
  virtual bool NotifyMouseButton(
      int index, bool down, const InputModifiers& modifiers) OVERRIDE;
  virtual bool NotifyKey(
      InputKey key, bool down, const InputModifiers& modifiers) OVERRIDE;
  virtual bool WantMouseEvents() OVERRIDE { return true; }
  virtual bool WantKeyEvents() OVERRIDE { return true; }

  // Implementation of ScrollHelperDataProvider:
  virtual int GetContentSize() OVERRIDE;
  virtual const Rect& GetScreenRect() OVERRIDE;

 private:
  bool LineInView(int line_number);
  int GetFirstLineInView();
  std::vector<string16> lines_;
  ScrollHelper scroll_helper_;
};

#endif  // SG_UI_SCROLLING_OUTPUT_VIEW_H_
