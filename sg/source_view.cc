// Copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "sg/source_view.h"

#include <algorithm>

#include "base/bind.h"
#include "base/file_util.h"
#include "base/logging.h"
#include "base/string_number_conversions.h"
#include "base/string_piece.h"
#include "base/utf_string_conversions.h"
#include "Gwen/Gwen.h"
#include "Gwen/Texture.h"
#include "sg/app_thread.h"
#include "sg/cpp_lexer.h"
#include "sg/lexer.h"
#include "sg/ui/skin.h"

namespace {

// TODO(config):
// TODO(rendering): Font line height.
const int g_line_height = 17;
Gwen::Texture g_pc_indicator_texture;
Gwen::Texture g_breakpoint_texture;


// TODO(scottmg): Losing last line if doesn't end in \n.
void SyntaxHighlight(const std::string& input, std::vector<Line>* lines) {
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

std::vector<Line> HighlightOnFILE(std::string utf8_text) {
  std::vector<Line> into;
  // Sleep(10000);
  SyntaxHighlight(utf8_text, &into);
  return into;
}

}  // namespace

SourceView::SourceView(const Skin& skin)
    : Contents(skin),
      scroll_helper_(skin, this, g_line_height),
      program_counter_line_(-1) {
  font_.facename = L"Consolas";
  font_.size = 13.f;
  g_pc_indicator_texture.name = "art/pc-location.png";
  g_breakpoint_texture.name = "art/breakpoint.png";
}

void SourceView::SetData(const std::string& utf8_text) {
  lines_.clear();

  // Add placeholder while we're processing.
  // TODO(jank): The file access happens on FILE too, but we could push the
  // placeholder from when we first queue the request to load the file.
  ColoredText segment;
  segment.type = Lexer::Text;
  segment.text = L"Loading...";
  Line line;
  line.push_back(segment);
  lines_.push_back(line);

  // TODO(scottmg): There's a crazy amount of by-value copying going on here.
  // Better than on-thread, but probably should RefPtr some structures instead.
  AppThread::PostTaskAndReplyWithResult(AppThread::FILE, FROM_HERE,
      base::Bind(&HighlightOnFILE,
                 utf8_text),
      base::Bind(&SourceView::CommitAfterHighlight, base::Unretained(this)));
  Invalidate();
}

void SourceView::SetProgramCounterLine(int line_number) {
  program_counter_line_ = line_number - 1;
  Invalidate();
}

void SourceView::CommitAfterHighlight(std::vector<Line> lines) {
  lines_ = lines;
  Invalidate();
}

int SourceView::GetFirstLineInView() {
  return scroll_helper_.GetOffset() / g_line_height;
}

bool SourceView::LineInView(int line_number) {
  int start_line = GetFirstLineInView();
  if (line_number < start_line)
    return false;
  if ((line_number - start_line) * g_line_height > Height() + g_line_height)
    return false;
  return true;
}

// TODO(rendering): Brutal efficiency.
void SourceView::Render(Gwen::Renderer::Base* renderer) {
  const Skin& skin = Contents::GetSkin();

  // TODO(rendering): Hacky.
  if (!g_pc_indicator_texture.data) {
    renderer->LoadTexture(&g_pc_indicator_texture);
  }
  if (!g_breakpoint_texture.data) {
    renderer->LoadTexture(&g_breakpoint_texture);
  }

  // TODO(rendering): Need to separate Update/Render.
  bool invalidate = scroll_helper_.Update();
  if (invalidate)
    Invalidate();

  renderer->SetDrawColor(skin.GetColorScheme().background());
  renderer->DrawFilledRect(Gwen::Rect(0, 0, Width(), Height()));
  int start_line = GetFirstLineInView();

  // Not quite right, but probably close enough.
  int largest_numbers_width = renderer->MeasureText(
      &font_,
      base::IntToString16(lines_.size()).c_str()).x;
  static const int left_margin = 5;
  static const int right_margin = 10;
  static const int indicator_width = g_line_height;
  static const int indicator_height = g_line_height;
  static const int indicator_and_margin = indicator_width + 5;
  renderer->SetDrawColor(skin.GetColorScheme().margin());
  renderer->DrawFilledRect(Gwen::Rect(
      0, 0,
      left_margin + largest_numbers_width + right_margin + indicator_and_margin,
      Height()));

  int y_pixel_scroll = scroll_helper_.GetOffset();

  for (size_t i = start_line; i < lines_.size(); ++i) {
    // Extra |g_line_height| added to height so that a full line is drawn at
    // the bottom when partial-line pixel scrolled.
    if (!LineInView(i))
      break;

    // Line numbers.
    renderer->SetDrawColor(skin.GetColorScheme().margin_text());
    renderer->RenderText(
        &font_,
        Gwen::Point(left_margin, i * g_line_height - y_pixel_scroll),
        base::IntToString16(i + 1).c_str());
    size_t x = left_margin + largest_numbers_width + right_margin +
               indicator_and_margin;

    // Source.
    for (size_t j = 0; j < lines_[i].size(); ++j) {
      renderer->SetDrawColor(ColorForTokenType(skin, lines_[i][j].type));
      renderer->RenderText(
          &font_,
          Gwen::Point(x, i * g_line_height - y_pixel_scroll),
          lines_[i][j].text.c_str());
      x += renderer->MeasureText(
          &font_,
          lines_[i][j].text.c_str()).x;
    }
  }

  if (LineInView(program_counter_line_)) {
    int y = program_counter_line_ * g_line_height - y_pixel_scroll;
    renderer->SetDrawColor(skin.GetColorScheme().pc_indicator());
    renderer->DrawTexturedRect(
        &g_pc_indicator_texture,
        Gwen::Rect(left_margin + largest_numbers_width + right_margin, y,
                   indicator_width, indicator_height),
        0, 0, 1, 1);
  }

  // Ease to target.
  scroll_helper_.RenderScrollIndicators(renderer);
}

bool SourceView::NotifyMouseMoved(
    int x, int y, int dx, int dy, const InputModifiers& modifiers) {
  return false;
}

bool SourceView::NotifyMouseWheel(int delta, const InputModifiers& modifiers) {
  if (scroll_helper_.ScrollPixels(-delta * .5f))  // TODO(config): Random scale.
    Invalidate();
  return true;
}

bool SourceView::NotifyKey(
    InputKey key, bool down, const InputModifiers& modifiers) {
  if (!down)
    return false;
  bool handled = false;
  bool invalidate = false;
  if (key == kDown) {
    invalidate = scroll_helper_.ScrollLines(1);
    handled = true;
  } else if (key == kUp) {
    invalidate = scroll_helper_.ScrollLines(-1);
    handled = true;
  } else if (key == kPageUp || (key == kSpace && modifiers.ShiftPressed())) {
    invalidate = scroll_helper_.ScrollPages(-1);
    handled = true;
  } else if (key == kPageDown || (key == kSpace && !modifiers.ShiftPressed())) {
    invalidate = scroll_helper_.ScrollPages(1);
    handled = true;
  } else if (key == kHome) {
    invalidate = scroll_helper_.ScrollToBeginning();
    handled = true;
  } else if (key == kEnd) {
    invalidate = scroll_helper_.ScrollToEnd();
    handled = true;
  }
  if (invalidate)
    Invalidate();
  return handled;
}

int SourceView::GetContentSize() {
  return g_line_height * lines_.size();
}

const Rect& SourceView::GetScreenRect() {
  return Contents::GetScreenRect();
}

const Color& SourceView::ColorForTokenType(
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
