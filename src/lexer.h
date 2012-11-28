#ifndef LEXER_H_
#define LEXER_H_

#include <string>
#include <map>
using namespace std;

class LexerState;

class Lexer {
 public:
  explicit Lexer(const string& name);
  LexerState* AddState(const string& name);

  enum TokenTypes {
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
  };

 private:
  map<string, LexerState*> states_;
};

#endif
