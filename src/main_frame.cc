#include "main_frame.h"

#include "base/basictypes.h"
#include "Gwen/Controls/Layout/Position.h"
#include "Gwen/Controls/ListBox.h"
#include "Gwen/Controls/TabControl.h"
#include "Gwen/Controls/WindowControl.h"
#include "Gwen/Platform.h"

using namespace Gwen;

GWEN_CONTROL_CONSTRUCTOR(MainFrame) {
  Dock(Pos::Fill);

  text_output_ = new Controls::ListBox(GetBottom());
  GetBottom()->GetTabControl()->AddPage("Output", text_output_);
  GetBottom()->SetHeight(200);

  Controls::Layout::Center* center = new Controls::Layout::Center(this);
  center->Dock(Pos::Fill);

  PrintText(L"Ready.\n");
}

void MainFrame::PrintText(const Gwen::UnicodeString& str) {
  text_output_->AddItem(str);
  text_output_->ScrollToBottom();
}
