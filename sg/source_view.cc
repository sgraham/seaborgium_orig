// Copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "sg/source_view.h"

#include <algorithm>

#include "base/bind.h"
#include "base/logging.h"
#include "base/string_number_conversions.h"
#include "base/string_piece.h"
#include "base/utf_string_conversions.h"
#include "sg/app_thread.h"
#include "sg/cpp_lexer.h"
#include "sg/lexer.h"
#include "sg/render/texture.h"
#include "sg/render/renderer.h"
#include "sg/ui/skin.h"

namespace {


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

SourceView::SourceView()
    : scroll_helper_(this, Skin::current().text_line_height()),
      program_counter_line_(-1) {
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
  return scroll_helper_.GetOffset() / Skin::current().text_line_height();
}

bool SourceView::LineInView(int line_number) {
  int start_line = GetFirstLineInView();
  int line_height = Skin::current().text_line_height();
  if (line_number < start_line)
    return false;
  if ((line_number - start_line) * line_height > Height() + line_height)
    return false;
  return true;
}

// TODO(rendering): Brutal efficiency.
void SourceView::Render(Renderer* renderer) {
  const Skin& skin = Skin::current();
  int line_height = skin.text_line_height();

  // TODO(rendering): Need to separate Update/Render.
  bool invalidate = scroll_helper_.Update();
  if (invalidate)
    Invalidate();

  renderer->SetDrawColor(skin.GetColorScheme().background());
  renderer->DrawFilledRect(Rect(0, 0, Width(), Height()));
  int start_line = GetFirstLineInView();

  // Not quite right, but probably close enough.
  int largest_numbers_width = renderer->MeasureText(
      skin.mono_font(),
      base::IntToString16(lines_.size()).c_str()).x;
  static const int left_margin = 5;
  static const int right_margin = 10;
  static const int indicator_width = line_height;
  static const int indicator_height = line_height;
  static const int indicator_and_margin = indicator_width + 5;
  renderer->SetDrawColor(skin.GetColorScheme().margin());
  renderer->DrawFilledRect(Rect(
      0, 0,
      left_margin + largest_numbers_width + right_margin + indicator_and_margin,
      Height()));

  int y_pixel_scroll = scroll_helper_.GetOffset();

  for (size_t i = start_line; i < lines_.size(); ++i) {
    // Extra |line_height| added to height so that a full line is drawn at
    // the bottom when partial-line pixel scrolled.
    if (!LineInView(i))
      break;

    // Line numbers.
    renderer->SetDrawColor(skin.GetColorScheme().margin_text());
    renderer->RenderText(
        skin.mono_font(),
        Point(left_margin, i * line_height - y_pixel_scroll),
        base::IntToString16(i + 1).c_str());
    size_t x = left_margin + largest_numbers_width + right_margin +
               indicator_and_margin;

    // Source.
    for (size_t j = 0; j < lines_[i].size(); ++j) {
      renderer->SetDrawColor(ColorForTokenType(skin, lines_[i][j].type));
      renderer->RenderText(
          skin.mono_font(),
          Point(x, i * line_height - y_pixel_scroll),
          lines_[i][j].text.c_str());
      x += renderer->MeasureText(
          skin.mono_font(),
          lines_[i][j].text.c_str()).x;
    }
  }

  if (LineInView(program_counter_line_)) {
    int y = program_counter_line_ * line_height - y_pixel_scroll;
    renderer->SetDrawColor(skin.GetColorScheme().pc_indicator());
    renderer->DrawTexturedRect(
        skin.pc_indicator_texture(),
        Rect(left_margin + largest_numbers_width + right_margin, y,
                   indicator_width, indicator_height),
        0, 0, 1, 1);
  }

  scroll_helper_.RenderScrollIndicators(renderer, skin);
}

bool SourceView::NotifyMouseMoved(
    int x, int y, int dx, int dy, const InputModifiers& modifiers) {
  return false;
}

bool SourceView::NotifyMouseWheel(int delta, const InputModifiers& modifiers) {
  bool invalidate, handled;
  scroll_helper_.CommonMouseWheel(delta, modifiers, &invalidate, &handled);
  if (invalidate)
    Invalidate();
  return handled;
}

bool SourceView::NotifyMouseButton(
    int index, bool down, const InputModifiers& modifiers) {
  // TODO(scottmg): Selection, data view, etc.
  return false;
}

bool SourceView::NotifyKey(
    InputKey key, bool down, const InputModifiers& modifiers) {
  bool invalidate, handled;
  scroll_helper_.CommonNotifyKey(key, down, modifiers, &invalidate, &handled);
  if (invalidate)
    Invalidate();
  return handled;
}

int SourceView::GetContentSize() {
  return Skin::current().text_line_height() * lines_.size();
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
