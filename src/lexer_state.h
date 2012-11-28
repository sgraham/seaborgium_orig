#ifndef LEXER_STATE_H_
#define LEXER_STATE_H_

#include "base/basictypes.h"
#include "lexer.h"

#include <vector>

class LexerState {
 public:
  LexerState(const std::string& name);

  void Add(const std::string& regex, Lexer::TokenType token_type);
  void AddWithTransition(const std::string& regex,
                         Lexer::TokenType token_type,
                         LexerState* transition_to);

 private:
  class ItemData {
   public:
    ItemData() : action(Lexer::Invalid), transition_to(NULL) {
    }

    ItemData(const std::string& regex,
             Lexer::TokenType action,
             LexerState* transition_to) :
        regex(regex),
        action(action),
        transition_to(transition_to) {
    }

    std::string regex;
    Lexer::TokenType action;
    LexerState* transition_to;
  };

  std::vector<ItemData> items;

  std::string name_;

  DISALLOW_COPY_AND_ASSIGN(LexerState);
};

#endif  // LEXER_STATE_H_
