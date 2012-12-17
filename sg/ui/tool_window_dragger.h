// Copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef SG_UI_TOOL_WINDOW_DRAGGER_H_
#define SG_UI_TOOL_WINDOW_DRAGGER_H_

#include "base/compiler_specific.h"
#include "base/memory/scoped_ptr.h"
#include "sg/basic_geometric_types.h"
#include "sg/ui/draggable.h"

class DockingToolWindow;
class Renderer;
class RenderToTextureRenderer;

class ToolWindowDragger : public Draggable {
 public:
  ToolWindowDragger(DockingToolWindow* dragging, const Point& screen_position);
  virtual ~ToolWindowDragger();

  virtual void Drag(const Point& screen_point) OVERRIDE;
  virtual void CancelDrag() OVERRIDE;
  virtual void Render(Renderer* renderer) OVERRIDE;

 private:
  DockingToolWindow* dragging_;
  Point pick_up_offset_;
  Point current_position_;
  scoped_ptr<RenderToTextureRenderer> render_to_texture_renderer_;
};

#endif  // SG_UI_TOOL_WINDOW_DRAGGER_H_