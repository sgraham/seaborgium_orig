// Copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef SG_SOURCE_VIEW_H_
#define SG_SOURCE_VIEW_H_

#include <string>
#include <vector>

#include "base/string16.h"
#include "Gwen/Controls/Base.h"
#include "Gwen/Font.h"
#include "Gwen/Structures.h"
#include "sg/lexer.h"
#include "sg/ui/contents.h"

namespace Gwen { namespace Skin { class Base; }}
class Skin;

struct ColoredText {
  Lexer::TokenType type;
  string16 text;
};
typedef std::vector<ColoredText> Line;

class SourceView : public Contents {
 public:
  explicit SourceView(const Skin& skin);

  virtual void Render(Gwen::Renderer::Base* renderer);

  virtual void SetData(const std::string& utf8_text);

  // Implementation of InputHandler.
  virtual bool NotifyMouseMoved(
      int x, int y, int dx, int dy, const InputModifiers& modifiers) OVERRIDE;
  virtual bool NotifyMouseWheel(
      int delta, const InputModifiers& modifiers) OVERRIDE;
  virtual bool NotifyKey(
      InputKey key, bool down, const InputModifiers& modifiers) OVERRIDE;
  virtual bool WantMouseEvents() { return true; }
  virtual bool WantKeyEvents() { return true; }

 private:

  const Gwen::Color& ColorForTokenType(const Skin& skin, Lexer::TokenType type);
  float GetLargestScrollLocation();
  void ClampScrollTarget();
  void ScrollView(int number_of_lines);
  void CommitAfterHighlight(std::vector<Line> lines);

  float y_pixel_scroll_;
  float y_pixel_scroll_target_;

  std::vector<Line> lines_;
  Gwen::Font font_;
};

#endif  // SG_SOURCE_VIEW_H_
