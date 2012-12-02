#include "ui/focus.h"

namespace {

Contents* g_focused;

}  // namespace

// TODO: Probably some sort of OnFocus/OnBlur?

Contents* GetFocusedContents() {
  return g_focused;
}

void SetFocusedContents(Contents* contents) {
  g_focused = contents;
}
