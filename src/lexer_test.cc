#include "lexer.h"
#include "lexer_state.h"

#include <gtest/gtest.h>

TEST(Lexer, Basic) {
  Lexer* lexer = new Lexer("test");
  LexerState* root = lexer->AddState("root");

  root->Add("a", Lexer::Keyword);
  root->Add("b", Lexer::KeywordConstant);
  root->Add("c", Lexer::KeywordPseudo);

  std::vector<Token> tokens;
  lexer->GetTokensUnprocessed("ababc", &tokens);
  EXPECT_EQ(0, tokens[0].index);
  EXPECT_EQ(Lexer::Keyword, tokens[0].token);
  EXPECT_EQ("a", tokens[0].value);
}
