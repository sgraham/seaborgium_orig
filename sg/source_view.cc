// Copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "sg/source_view.h"

#include <algorithm>

#include "Gwen/Gwen.h"
#include "base/file_util.h"
#include "base/logging.h"
#include "base/string_number_conversions.h"
#include "base/string_piece.h"
#include "base/utf_string_conversions.h"
#include "sg/cpp_lexer.h"
#include "sg/lexer.h"
#include "sg/ui/skin.h"

namespace {
const int g_line_height = 16;
}  // namespace

SourceView::SourceView(const Skin& skin)
    : Contents(skin) {
  y_pixel_scroll_ = 0.f;
  y_pixel_scroll_target_ = 0.f;
  font_.facename = L"Consolas";
  font_.size = 13.f;
  std::string utf8_text = "File\nload\nfailed!\n";
  /*
  if (file_util::ReadFileToString(
      FilePath(FILE_PATH_LITERAL("sample_source_code_file.cc")), &utf8_text)) {
  }
  */
  SyntaxHighlight(utf8_text, &lines_);
  /*
  SetKeyboardInputEnabled(true);
  Focus();
  */
}

// TODO(rendering): Brutal efficiency.
void SourceView::Render(Gwen::Renderer::Base* renderer) {
  const Skin& skin = Contents::GetSkin();

  // Ease to target.
  y_pixel_scroll_ += (y_pixel_scroll_target_ - y_pixel_scroll_) * 0.2f;
  if (fabsf(y_pixel_scroll_target_ - y_pixel_scroll_) < 1.f)
    y_pixel_scroll_ = y_pixel_scroll_target_;

  // TODO(rendering): X/Y should be unnecessary; Translate to correct
  // offset during hierarchy traversal.
  renderer->SetDrawColor(skin.GetColorScheme().background());
  renderer->DrawFilledRect(Gwen::Rect(X(), Y(), Width(), Height()));
  size_t start_line =
      std::max(0, static_cast<int>(y_pixel_scroll_ / g_line_height));

  // Not quite right, but probably close enough.
  int largest_numbers_width = renderer->MeasureText(
      &font_,
      base::IntToString16(lines_.size()).c_str()).x;
  static const int left_margin = 5;
  static const int right_margin = 15;
  renderer->SetDrawColor(skin.GetColorScheme().margin());
  renderer->DrawFilledRect(Gwen::Rect(
        X(), Y(), left_margin + largest_numbers_width + right_margin, Height()));

  for (size_t i = start_line; i < lines_.size(); ++i) {
    // Extra |g_line_height| added to height so that a full line is drawn at
    // the bottom when partial-line pixel scrolled.
    if ((i - start_line) * g_line_height > Height() + g_line_height - 500)
      break;

    // Line numbers.
    renderer->SetDrawColor(skin.GetColorScheme().margin_text());
    renderer->RenderText(
        &font_,
        Gwen::Point(left_margin, i * g_line_height - y_pixel_scroll_),
        base::IntToString16(i + 1).c_str());
    size_t x = left_margin + largest_numbers_width + right_margin;

    // Source.
    for (size_t j = 0; j < lines_[i].size(); ++j) {
      renderer->SetDrawColor(ColorForTokenType(skin, lines_[i][j].type));
      renderer->RenderText(
          &font_,
          Gwen::Point(x, i * g_line_height - y_pixel_scroll_),
          lines_[i][j].text.c_str());
      x += renderer->MeasureText(
          &font_,
          lines_[i][j].text.c_str()).x;
    }
  }
}

float SourceView::GetLargestScrollLocation() {
  return static_cast<float>((lines_.size() - 1) * g_line_height);
}

void SourceView::ClampScrollTarget() {
  y_pixel_scroll_target_ = std::max(0.f, y_pixel_scroll_target_);
  y_pixel_scroll_target_ = std::min(
      GetLargestScrollLocation(), y_pixel_scroll_target_);
}

bool SourceView::OnMouseWheeled(int delta) {
  y_pixel_scroll_target_ -= delta * .5f;  // TODO(scottmg): Random scale.
  ClampScrollTarget();
  return true;
}

void SourceView::ScrollView(int number_of_lines) {
  y_pixel_scroll_target_ =
    static_cast<int>(y_pixel_scroll_target_ / g_line_height) * g_line_height;
  y_pixel_scroll_target_ += g_line_height * number_of_lines;
  ClampScrollTarget();
}

// Note: These are the Up arrow and Down arrow keys, not press/release.
bool SourceView::OnKeyUp(bool down) {
  if (down)
    ScrollView(-1);
  return true;
}

bool SourceView::OnKeyDown(bool down) {
  if (down)
    ScrollView(1);
  return true;
}

bool SourceView::OnKeyPageUp(bool down) {
  if (down)
    ScrollView(-(Height() / g_line_height - 1));
  return true;
}

bool SourceView::OnKeyPageDown(bool down) {
  if (down)
    ScrollView(Height() / g_line_height - 1);
  return true;
}

bool SourceView::OnKeySpace(bool down) {
  if (down) {
    if (Gwen::Input::IsShiftDown())
      OnKeyPageUp(true);
    else
      OnKeyPageDown(true);
  }
  return true;
}

bool SourceView::OnKeyHome(bool down) {
  y_pixel_scroll_target_ = 0.f;
  return true;
}

bool SourceView::OnKeyEnd(bool down) {
  y_pixel_scroll_target_ = GetLargestScrollLocation();
  return true;
}

// TODO(scottmg): Losing last line if doesn't end in \n.
void SourceView::SyntaxHighlight(
    const std::string& input, std::vector<Line>* lines) {
  scoped_ptr<Lexer> lexer(MakeCppLexer());
  std::vector<Token> tokens;
  lexer->GetTokensUnprocessed(input, &tokens);
  Line current_line;
  for (size_t i = 0; i < tokens.size(); ++i) {
    const Token& token = tokens[i];
    if (token.value == "\n") {
      lines->push_back(current_line);
      current_line.clear();
    } else {
      ColoredText fragment;
      fragment.type = token.token;
      fragment.text = UTF8ToUTF16(token.value);
      current_line.push_back(fragment);
      if (token.token == Lexer::CommentSingle) {
        // Includes \n in its value so we don't otherwise see it.
        lines->push_back(current_line);
        current_line.clear();
      }
    }
  }
}

const Gwen::Color& SourceView::ColorForTokenType(
    const ::Skin& skin, Lexer::TokenType type) {
  // TODO(config): More customizability with fallbacks.
  const ColorScheme& cs = skin.GetColorScheme();
  switch (type) {
    case Lexer::Comment:
    case Lexer::CommentMultiline:
    case Lexer::CommentSingle:
      return cs.comment();
    case Lexer::CommentPreproc:
      return cs.comment_preprocessor();
    case Lexer::Error:
      return cs.error();
    case Lexer::Keyword:
    case Lexer::KeywordConstant:
    case Lexer::KeywordPseudo:
    case Lexer::KeywordReserved:
      return cs.keyword();
    case Lexer::KeywordType:
      return cs.keyword_type();
    case Lexer::LiteralNumberFloat:
    case Lexer::LiteralNumberHex:
    case Lexer::LiteralNumberInteger:
    case Lexer::LiteralNumberOct:
      return cs.literal_number();
    case Lexer::LiteralString:
    case Lexer::LiteralStringChar:
    case Lexer::LiteralStringEscape:
      return cs.literal_string();
    case Lexer::Name:
    case Lexer::NameBuiltin:
    case Lexer::NameLabel:
      break;
    case Lexer::NameClass:
      return cs.klass();
    case Lexer::Operator:
      return cs.op();
    case Lexer::Punctuation:
    case Lexer::Text:
    default:
      break;
  }
  return cs.text();
}
