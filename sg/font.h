// Copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef SG_FONT_H_
#define SG_FONT_H_

class Font {
 public:
  virtual ~Font() {}

  void SetFaceName(const string16& face_name) = 0;
  void SetSize(float size) = 0;
};

#endif  // SG_FONT_H_
