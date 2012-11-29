#include "source_view.h"

#include "base/file_util.h"
#include "base/string_piece.h"
#include "base/utf_string_conversions.h"
#include "cpp_lexer.h"
#include "lexer.h"
#include "Gwen/Gwen.h"

using namespace Gwen;
using namespace Gwen::Controls;

GWEN_CONTROL_CONSTRUCTOR(SourceView) {
  Dock(Pos::Fill);
  color_ = Colors::Black;
  string16 text = L"File\nload\nfailed!";
  std::string utf8_text;
  if (file_util::ReadFileToString(
      FilePath(FILE_PATH_LITERAL("sample_source_code_file.cc")), &utf8_text)) {
    SyntaxHighlight(utf8_text, &lines_);
  }
}

// TODO: Brutal efficiency.
void SourceView::Render(Skin::Base* skin) {
  GetCanvas()->SetBackgroundColor(Colors::White);
  Gwen::Renderer::Base* render = skin->GetRender();
  const size_t line_height = 13;
  for (size_t i = 0; i < lines_.size(); ++i) {
    if (i * line_height > Height())
      break;
    size_t x = 0;
    for (size_t j = 0; j < lines_[i].size(); ++j) {
      render->SetDrawColor(ColorForTokenType(lines_[i][j].type));
      render->RenderText(
          skin->GetDefaultFont(),
          Gwen::Point(x, i * line_height),
          lines_[i][j].text.c_str());
      x += render->MeasureText(
          skin->GetDefaultFont(),
          lines_[i][j].text.c_str()).x;
    }
  }
}

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

Gwen::Color SourceView::ColorForTokenType(Lexer::TokenType type) {
  // TODO: This is horribly inefficient.
  // TODO: User configurable.
  switch (type) {
    case Lexer::Comment:
    case Lexer::CommentMultiline:
    case Lexer::CommentSingle:
      return Gwen::Color(0x00, 0x80, 0x00);
    case Lexer::CommentPreproc:
      return Gwen::Color(0x00, 0x00, 0xff);
    case Lexer::Error:
      return Gwen::Color(0xff, 0x00, 0x00);
    case Lexer::Keyword:
    case Lexer::KeywordConstant:
    case Lexer::KeywordPseudo:
    case Lexer::KeywordReserved:
      return Gwen::Color(0x00, 0x00, 0xff);
    case Lexer::KeywordType:
      return Gwen::Color(0x2b, 0x91, 0xaf);
    case Lexer::LiteralNumberFloat:
    case Lexer::LiteralNumberHex:
    case Lexer::LiteralNumberInteger:
    case Lexer::LiteralNumberOct:
      return Gwen::Color(0x40, 0xa0, 0x70);
    case Lexer::LiteralString:
    case Lexer::LiteralStringChar:
    case Lexer::LiteralStringEscape:
      return Gwen::Color(0xa3, 0x15, 0x15);
    case Lexer::Name:
    case Lexer::NameBuiltin:
    case Lexer::NameLabel:
      break;
    case Lexer::NameClass:
      return Gwen::Color(0x2b, 0x91, 0xaf);
    case Lexer::Operator:
      return Gwen::Color(0x00, 0x00, 0xff);
    case Lexer::Punctuation:
    case Lexer::Text:
    default:
      break;
  }
  return Gwen::Colors::Black;
}
