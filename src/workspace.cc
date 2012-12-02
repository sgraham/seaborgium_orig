#include "workspace.h"

#include "ui/container.h"
#include "ui/solid_color.h"
#include "Gwen/Gwen.h"

Contents* Placeholder(const Skin& skin, const string16& name) {
  Container* container = new Container;
  container->AddChild(new SolidColor(skin.GetColorScheme().background()),
                      name);
  return container;
}

GWEN_CONTROL_CONSTRUCTOR(Workspace) {
  Dock(Gwen::Pos::Fill);
  root_ = new Container;

  Container* top = new Container;
  Container* bottom = new Container;
  root_->SetMode(Container::SplitVertical);
  root_->AddChild(top);
  root_->AddChild(bottom);
  root_->SetFraction(top, .7);

  Contents* output = Placeholder(skin_, L"Output");
  Contents* log = Placeholder(skin_, L"Log");
  bottom->AddChild(output);
  bottom->AddChild(log);
  bottom->SetFraction(output, .6);

  Contents* source = Placeholder(skin_, L"src\\blah_blah.cc");
  Container* middle = new Container;
  middle->SetMode(Container::SplitVertical);
  Container* views = new Container;
  views->SetMode(Container::SplitVertical);
  top->AddChild(source);
  top->AddChild(middle);
  top->AddChild(views);
  top->SetFraction(source, .4);
  top->SetFraction(middle, .65);

  Contents* stack = Placeholder(skin_, L"Stack");
  Contents* breakpoints = Placeholder(skin_, L"Breakpoints");
  middle->AddChild(stack);
  middle->AddChild(breakpoints);
  middle->SetFraction(stack, .75);

  Contents* watch = Placeholder(skin_, L"Watch");
  Contents* locals = Placeholder(skin_, L"Locals");
  views->AddChild(watch);
  views->AddChild(locals);
  views->SetFraction(watch, .6);
}

void Workspace::Render(Gwen::Skin::Base* gwen_skin) {
  root_->SetScreenRect(Rect(0, 0, Width(), Height()));
  root_->Render(skin_, gwen_skin->GetRender());
}
