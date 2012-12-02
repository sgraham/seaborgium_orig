#include "workspace.h"

#include "ui/container.h"
#include "ui/focus.h"
#include "ui/solid_color.h"
#include "source_view.h"
#include "Gwen/Gwen.h"

namespace {

Container* Placeholder(const Skin& skin, const string16& name) {
  Container* container = new Container;
  container->AddChild(new SolidColor(skin.GetColorScheme().background()),
                      name);
  return container;
}

Container* MakeSourceView(Gwen::Controls::Base* parent, const string16& source_file) {
  Container* container = new Container;
  SourceView* source_view = new SourceView(parent);
  container->AddChild(source_view, source_file);
  return container;
}

}  // namespace

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

  Container* source = MakeSourceView(this, L"sample_source_code_file.cc");
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

  SetFocusedContents(source->Child(0));
}

Workspace::~Workspace() {
  delete root_;
}

void Workspace::Render(Gwen::Skin::Base* gwen_skin) {
  root_->SetScreenRect(Rect(0, 0, Width(), Height()));
  root_->Render(skin_, gwen_skin->GetRender());
}
