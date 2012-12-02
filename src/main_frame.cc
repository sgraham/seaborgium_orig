#include "main_frame.h"

#include "base/basictypes.h"
#include "Gwen/Controls/Layout/Position.h"
#include "Gwen/Controls/ListBox.h"
#include "Gwen/Controls/TabControl.h"
#include "Gwen/Controls/WindowControl.h"
#include "Gwen/Platform.h"
#include "source_view.h"

using namespace Gwen;

GWEN_CONTROL_CONSTRUCTOR(MainFrame) {
  Dock(Pos::Fill);

  text_output_ = new Controls::ListBox(GetBottom());
  GetBottom()->GetTabControl()->AddPage("Output", text_output_);
  GetBottom()->SetHeight(200);

  breakpoints_ = new Controls::ListBox(GetBottom());
  GetRight()->GetTabControl()->AddPage("Breakpoints", breakpoints_);
  GetRight()->SetHeight(300);
  breakpoints_->AddItem(L"DockBase.cpp @ 13");

  call_stack_ = new Controls::ListBox(GetBottom());
  GetRight()->GetTabControl()->AddPage("Call Stack", call_stack_);
  GetRight()->SetHeight(300);
  call_stack_->AddItem(L"int func() a.cc, line 15");
  call_stack_->AddItem(L"int main() main.cc, line 245");

  Controls::Layout::Center* center = new Controls::Layout::Center(this);
  center->Dock(Pos::Fill);

  new SourceView(center);

  PrintText(L"Ready.\n");
}

void MainFrame::PrintText(const Gwen::UnicodeString& str) {
  text_output_->AddItem(str);
  text_output_->ScrollToBottom();
}
