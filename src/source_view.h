#ifndef SOURCE_VIEW_H_
#define SOURCE_VIEW_H_

#include "base/string16.h"
#include "Gwen/Controls/Base.h"
#include "Gwen/Structures.h"
#include "lexer.h"

#include <vector>

namespace Gwen { namespace Skin { class Base; }}

class SourceView : public Gwen::Controls::Base {
 public:
  GWEN_CONTROL(SourceView, Gwen::Controls::Base);

  virtual void Render(Gwen::Skin::Base* skin);

  virtual bool OnMouseWheeled(int delta);

 private:
  struct ColoredText {
    Lexer::TokenType type;
    string16 text;
  };
  typedef std::vector<ColoredText> Line;

  void SyntaxHighlight(const std::string& input, std::vector<Line>* lines);
  Gwen::Color ColorForTokenType(Lexer::TokenType type);

  float y_pixel_scroll_;
  float y_pixel_scroll_target_;

  std::vector<Line> lines_;
};

#endif  // SOURCE_VIEW_H_
