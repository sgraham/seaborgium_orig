#include "lexer_state.h"

LexerState::LexerState(const std::string& name) : name_(name) {
}

void LexerState::Add(const std::string& regex, Lexer::TokenType token_type) {
  items.push_back(ItemData(regex, token_type, NULL));
}

void LexerState::AddWithTransition(const std::string& regex,
                                   Lexer::TokenType token_type,
                                   LexerState* transition_to) {
  items.push_back(ItemData(regex, token_type, transition_to));
}
