#include "top_level_frame.h"

#include "Gwen/Controls/Layout/Position.h"
#include "Gwen/Controls/ListBox.h"
#include "Gwen/Controls/TabControl.h"
#include "Gwen/Controls/WindowControl.h"
#include "Gwen/Platform.h"
#include "main_frame.h"

using namespace Gwen;

GWEN_CONTROL_CONSTRUCTOR(TopLevelFrame) {
  Dock(Pos::Fill);

  status_bar_ = new Controls::StatusBar(this);
  status_bar_->Dock(Pos::Bottom);

  main_frame_ = new MainFrame(this);
  main_frame_->Dock(Pos::Fill);
  status_bar_->SendToBack();

  last_second_ = Gwen::Platform::GetTimeInSeconds();
  frames_ = 0;
}

void TopLevelFrame::Render(Gwen::Skin::Base* skin) {
  frames_++;

  if (last_second_ < Gwen::Platform::GetTimeInSeconds()) {
    status_bar_->SetText(Gwen::Utility::Format(L"%i fps", frames_ * 2));
    last_second_ = Gwen::Platform::GetTimeInSeconds() + 0.5f;
    frames_ = 0;
  }

  BaseClass::Render(skin);
}
