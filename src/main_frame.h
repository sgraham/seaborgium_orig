#ifndef MAIN_FRAME_H_
#define MAIN_FRAME_H_

#include "Gwen/Gwen.h"
#include "Gwen/Align.h"
#include "Gwen/Utility.h"
#include "Gwen/Controls/DockBase.h"

class SourceView;
namespace Gwen { namespace Controls { class ListBox; } }

class MainFrame : public Gwen::Controls::DockBase {
 public:
  GWEN_CONTROL(MainFrame, Gwen::Controls::DockBase);
  void PrintText(const Gwen::UnicodeString& str);

 private:
  Gwen::Controls::ListBox* text_output_;
  SourceView* source_view_;
};

#endif  // MAIN_FRAME_H_
