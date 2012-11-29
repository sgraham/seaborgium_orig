#include "lexer.h"

#include "cpp_lexer.h"
#include "lexer_state.h"

#include <gtest/gtest.h>

TEST(Lexer, Basic) {
  Lexer* lexer = new Lexer("test");
  LexerState* root = lexer->AddState("root");

  TokenDefinitions defs;
  defs.Add("a", Lexer::Keyword);
  defs.Add("b", Lexer::KeywordConstant);
  defs.Add("c", Lexer::KeywordPseudo);
  root->SetTokenDefinitions(defs);

  std::vector<Token> tokens;
  lexer->GetTokensUnprocessed("ababc", &tokens);

  EXPECT_EQ(5, tokens.size());

  EXPECT_EQ(0, tokens[0].index);
  EXPECT_EQ(Lexer::Keyword, tokens[0].token);
  EXPECT_EQ("a", tokens[0].value);

  EXPECT_EQ(1, tokens[1].index);
  EXPECT_EQ(Lexer::KeywordConstant, tokens[1].token);
  EXPECT_EQ("b", tokens[1].value);

  EXPECT_EQ(2, tokens[2].index);
  EXPECT_EQ(Lexer::Keyword, tokens[2].token);
  EXPECT_EQ("a", tokens[2].value);

  EXPECT_EQ(3, tokens[3].index);
  EXPECT_EQ(Lexer::KeywordConstant, tokens[3].token);
  EXPECT_EQ("b", tokens[3].value);

  EXPECT_EQ(4, tokens[4].index);
  EXPECT_EQ(Lexer::KeywordPseudo, tokens[4].token);
  EXPECT_EQ("c", tokens[4].value);
}

TEST(Lexer, IniFile) {
  Lexer* lexer = new Lexer("ini-ish");
  LexerState* root = lexer->AddState("root");

  TokenDefinitions defs;
  defs.Add("\\s+", Lexer::Text);
  defs.Add("[;#].*", Lexer::CommentSingle);
  defs.Add("\\[.*?\\]", Lexer::Keyword);

  root->SetTokenDefinitions(defs);

  std::vector<Token> tokens;
  lexer->GetTokensUnprocessed("[wee]\n; stuff\n# things\n    \n", &tokens);

  EXPECT_EQ(6, tokens.size());
  // TODO
}

TEST(Lexer, Cpp) {
  Lexer* lexer = MakeCppLexer();

  std::vector<Token> tokens;
  lexer->GetTokensUnprocessed("int foo;", &tokens);
  EXPECT_EQ(4, tokens.size());

  EXPECT_EQ(Lexer::KeywordType, tokens[0].token);
  EXPECT_EQ(0, tokens[0].index);

  EXPECT_EQ(Lexer::Text, tokens[1].token);
  EXPECT_EQ(3, tokens[1].index);

  EXPECT_EQ(Lexer::Name, tokens[2].token);
  EXPECT_EQ(4, tokens[2].index);

  EXPECT_EQ(Lexer::Punctuation, tokens[3].token);
  EXPECT_EQ(7, tokens[3].index);
}
