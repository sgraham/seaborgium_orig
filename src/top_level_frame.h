#ifndef TOP_LEVEL_FRAME_H_
#define TOP_LEVEL_FRAME_H_

#include "Gwen/Gwen.h"
#include "Gwen/Align.h"
#include "Gwen/Utility.h"
#include "Gwen/Controls/DockBase.h"
#include "Gwen/Controls/StatusBar.h"

namespace Gwen { namespace Controls { class ListBox; } }
class MainFrame;

// Contains a status bar and a docking container
class TopLevelFrame : public Gwen::Controls::Base {
 public:
  GWEN_CONTROL(TopLevelFrame, Gwen::Controls::Base);
  void Render(Gwen::Skin::Base* skin);

 private:
  MainFrame* main_frame_;
  Gwen::Controls::StatusBar* status_bar_;
  unsigned int frames_;
  float last_second_;
};

#endif  // TOP_LEVEL_FRAME_H_
