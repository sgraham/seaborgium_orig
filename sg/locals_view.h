// Copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef SG_LOCALS_VIEW_H_
#define SG_LOCALS_VIEW_H_

#include <vector>

#include "base/string16.h"
#include "sg/backend/backend.h"
#include "sg/render/font.h"
#include "sg/ui/contents.h"

class LocalsView : public Contents {
 public:
  LocalsView();

  virtual void Render(Renderer* renderer);

  virtual void SetData(const std::vector<TypeNameValue>& locals);

 private:
  std::vector<string16> lines_;
};

#endif  // SG_LOCALS_VIEW_H_
