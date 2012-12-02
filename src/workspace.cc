#include "workspace.h"

#include "container.h"
#include "Gwen/Gwen.h"

GWEN_CONTROL_CONSTRUCTOR(Workspace) {
  Dock(Gwen::Pos::Fill);
  root_ = new Container;
  Container* a = new Container;
  root_->AddChild(a);
  Container* b = new Container;
  root_->AddChild(b);
  Container* c = new Container;
  root_->AddChild(c);
  Container* d = new Container;
  d->SetMode(Container::SplitVertical);
  root_->AddChild(d);
  a->AddChild(new SolidColor(skin_.GetColorScheme().background()));
  b->AddChild(new SolidColor(skin_.GetColorScheme().background()));
  c->AddChild(new SolidColor(skin_.GetColorScheme().background()));
  Container* x = new Container;
  d->AddChild(x);
  Container* y = new Container;
  d->AddChild(y);
  d->SetFraction(x, 0.21);
  x->AddChild(new SolidColor(skin_.GetColorScheme().background()));
  y->AddChild(new SolidColor(skin_.GetColorScheme().background()));
}

void Workspace::Render(Gwen::Skin::Base* gwen_skin) {
  root_->SetScreenRect(Rect(0, 0, Width(), Height()));
  root_->Render(skin_, gwen_skin->GetRender());
}
