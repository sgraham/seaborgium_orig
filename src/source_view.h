#ifndef SOURCE_VIEW_H_
#define SOURCE_VIEW_H_

#include "base/string16.h"
#include "Gwen/Controls/Base.h"
#include "Gwen/Font.h"
#include "Gwen/Structures.h"
#include "lexer.h"

#include <vector>

namespace Gwen { namespace Skin { class Base; }}

class SourceView : public Gwen::Controls::Base {
 public:
  GWEN_CONTROL(SourceView, Gwen::Controls::Base);

  virtual void Render(Gwen::Skin::Base* skin);

  virtual bool OnMouseWheeled(int delta);
  virtual bool OnKeyHome(bool down);
  virtual bool OnKeyEnd(bool down);
  virtual bool OnKeyUp(bool down);
  virtual bool OnKeyDown(bool down);
  virtual bool OnKeyPageUp(bool down);
  virtual bool OnKeyPageDown(bool down);
  virtual bool OnKeySpace(bool down);

 private:
  struct ColoredText {
    Lexer::TokenType type;
    string16 text;
  };
  typedef std::vector<ColoredText> Line;

  void SyntaxHighlight(const std::string& input, std::vector<Line>* lines);
  Gwen::Color ColorForTokenType(Lexer::TokenType type);
  float GetLargestScrollLocation();
  void ClampScrollTarget();
  void ScrollView(int number_of_lines);

  float y_pixel_scroll_;
  float y_pixel_scroll_target_;

  std::vector<Line> lines_;
  Gwen::Font font_;
};

#endif  // SOURCE_VIEW_H_
