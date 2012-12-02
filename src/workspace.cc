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
  a->AddChild(new SolidColor(Gwen::Color(0, 0, 128)));
  b->AddChild(new SolidColor(Gwen::Color(128, 0, 128)));
  c->AddChild(new SolidColor(Gwen::Color(128, 128, 128)));
  Container* x = new Container;
  d->AddChild(x);
  Container* y = new Container;
  d->AddChild(y);
  x->AddChild(new SolidColor(Gwen::Color(128, 128, 0)));
  y->AddChild(new SolidColor(Gwen::Color(0, 128, 128)));
}

void Workspace::Render(Gwen::Skin::Base* skin) {
  root_->SetScreenRect(Rect(0, 0, Width(), Height()));
  root_->Render(skin->GetRender());
}
