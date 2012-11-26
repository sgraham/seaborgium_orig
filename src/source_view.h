#ifndef SOURCE_VIEW_H_
#define SOURCE_VIEW_H_

#include "base/string16.h"
#include "Gwen/Controls/Base.h"
#include "Gwen/Structures.h"

#include <vector>

namespace Gwen { namespace Skin { class Base; }}

class SourceView : public Gwen::Controls::Base {
 public:
  GWEN_CONTROL(SourceView, Gwen::Controls::Base);

  virtual void Render(Gwen::Skin::Base* skin);

 private:
  Gwen::Color color_;
  std::vector<string16> lines_;
};

#endif  // SOURCE_VIEW_H_
