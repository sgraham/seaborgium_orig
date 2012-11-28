#include "lexer.h"

#include "lexer_state.h"

LexerState* Lexer::Push;
LexerState* Lexer::Pop;

Lexer::Lexer(const std::string& name) : name_(name) {
  if (!Push) {
    Push = new LexerState("!<push>");
    Pop = new LexerState("!<pop>");
  }
}

LexerState* Lexer::AddState(const std::string& name) {
  LexerState* lexer_state = new LexerState(name);
  states_[name] = lexer_state;
  return lexer_state;
}

void Lexer::GetTokensUnprocessed(const std::string& input,
                                 std::vector<Token>* output_tokens) {
  output_tokens->push_back(Token(0, Lexer::Invalid, ""));
}
