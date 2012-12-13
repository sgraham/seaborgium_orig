// Copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef SG_STACK_VIEW_H_
#define SG_STACK_VIEW_H_

#include <vector>

#include "base/string16.h"
#include "Gwen/Controls/Base.h"
#include "Gwen/Font.h"
#include "Gwen/Structures.h"
#include "sg/backend/backend.h"
#include "sg/ui/contents.h"

namespace Gwen { namespace Skin { class Base; }}
class Skin;

class StackView : public Contents {
 public:
  explicit StackView(const Skin& skin);

  virtual void Render(Gwen::Renderer::Base* renderer);

  virtual void SetData(const std::vector<FrameData>& frames, int active);

 private:
  std::vector<string16> lines_;
  int active_;

  Gwen::Font font_;
};

#endif  // SG_STACK_VIEW_H_
