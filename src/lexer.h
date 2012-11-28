#ifndef LEXER_H_
#define LEXER_H_

#include "base/basictypes.h"

#include <string>
#include <map>

class LexerState;

class Lexer {
 public:
  explicit Lexer(const std::string& name);
  LexerState* AddState(const std::string& name);

  enum TokenType {
    Comment,
    CommentMultiline,
    CommentPreproc,
    CommentSingle,
    Error,
    Keyword,
    KeywordConstant,
    KeywordPseudo,
    KeywordReserved,
    KeywordType,
    LiteralNumberFloat,
    LiteralNumberHex,
    LiteralNumberInteger,
    LiteralNumberOct,
    LiteralString,
    LiteralStringChar,
    LiteralStringEscape,
    Name,
    NameBuiltin,
    NameClass,
    NameLabel,
    Operator,
    Punctuation,
    Text,

    Invalid,
  };

  static LexerState* Push;
  static LexerState* Pop;

 private:
  std::string name_;
  std::map<std::string, LexerState*> states_;

  DISALLOW_COPY_AND_ASSIGN(Lexer);
};

#endif  // LEXER_H_
