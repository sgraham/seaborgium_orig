// Copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef SG_SOURCE_VIEW_H_
#define SG_SOURCE_VIEW_H_

#include <string>
#include <vector>

#include "base/string16.h"
#include "ui/contents.h"
#include "Gwen/Controls/Base.h"
#include "Gwen/Font.h"
#include "Gwen/Structures.h"
#include "sg/lexer.h"

namespace Gwen { namespace Skin { class Base; }}
class Skin;

class SourceView : public Gwen::Controls::Base, public Contents {
 public:
  GWEN_CONTROL(SourceView, Gwen::Controls::Base);

  virtual void Render(Gwen::Skin::Base* skin);
  virtual void Render(const Skin& skin, Gwen::Renderer::Base* renderer);

  virtual bool OnMouseWheeled(int delta);
  virtual bool OnKeyHome(bool down);
  virtual bool OnKeyEnd(bool down);
  virtual bool OnKeyUp(bool down);
  virtual bool OnKeyDown(bool down);
  virtual bool OnKeyPageUp(bool down);
  virtual bool OnKeyPageDown(bool down);
  virtual bool OnKeySpace(bool down);

 private:
  struct ColoredText {
    Lexer::TokenType type;
    string16 text;
  };
  typedef std::vector<ColoredText> Line;

  void SyntaxHighlight(const std::string& input, std::vector<Line>* lines);
  const Gwen::Color& ColorForTokenType(const Skin& skin, Lexer::TokenType type);
  float GetLargestScrollLocation();
  void ClampScrollTarget();
  void ScrollView(int number_of_lines);

  float y_pixel_scroll_;
  float y_pixel_scroll_target_;

  std::vector<Line> lines_;
  Gwen::Font font_;
};

#endif  // SG_SOURCE_VIEW_H_
