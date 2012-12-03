// Copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef SG_WORKSPACE_H_
#define SG_WORKSPACE_H_

#include <string>

#include "sg/debug_presenter_display.h"
#include "sg/ui/container.h"
#include "sg/ui/skin.h"

class ApplicationWindow;
class Container;
class SourceView;

class Workspace : public Container, public DebugPresenterDisplay {
 public:
  Workspace();
  virtual ~Workspace();

  virtual void SetDelegate(ApplicationWindow* delegate);

  virtual void Invalidate();

  // Implementation of DebugPresenterDisplay.
  virtual void SetFileName(const FilePath& filename) OVERRIDE;
  virtual void SetFileData(const std::string& utf8_text) OVERRIDE;

 private:
  Skin skin_;
  SourceView* source_view_;
  Container* source_view_container_;
  ApplicationWindow* delegate_;
};

#endif  // SG_WORKSPACE_H_
