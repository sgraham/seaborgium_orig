// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "sg/render/application_window.h"

#include "base/basictypes.h"
#include "base/bind.h"
#include "sg/app_thread.h"
#include "sg/basic_geometric_types.h"
#if 0
#include "sg/render/gpu.h"
#endif
#include "sg/ui/input.h"
#include "sg/workspace.h"

#include "SDL.h"

namespace {

class ApplicationWindowSdl : public ApplicationWindow {
 public:
  ApplicationWindowSdl()
      : workspace_(NULL),
        last_mouse_x_(0),
        last_mouse_y_(0) {
    CHECK_EQ(0, SDL_Init(SDL_INIT_VIDEO));  // TODO(sdl): Shouldn't be here.
    window_ = SDL_CreateWindow(
        "Seaborgium",
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        1280,
        1024,
        SDL_WINDOW_OPENGL | SDL_WINDOW_MAXIMIZED | SDL_WINDOW_RESIZABLE);
    CHECK(window_);
  }

  virtual ~ApplicationWindowSdl() {
  }

  virtual void Show() OVERRIDE {
    DCHECK(AppThread::CurrentlyOn(AppThread::UI));
#if 0
    SetWindowText(hwnd_, L"Seaborgium");
    SetCursor(LoadCursor(NULL, IDC_ARROW));
    ShowWindow(hwnd_, SW_SHOWMAXIMIZED);
    SetForegroundWindow(hwnd_);
    SetFocus(hwnd_);
#endif
  }

  virtual void Paint() OVERRIDE {
#if 0
    Gpu::Paint(this);
    ValidateRect(hwnd_, NULL);
#endif
  }

  virtual void SetDebugPresenterNotify(DebugPresenterNotify* notifier) {
#if 0
    Gpu::SetDebugPresenterNotify(this, notifier);
#endif
  }

  virtual void SetWorkspace(Workspace* workspace) {
    workspace_ = workspace;
  }
  virtual Workspace* GetWorkspace() { return workspace_; }

  virtual bool IsLandscape() {
    SDL_DisplayMode mode;
    CHECK(SDL_GetDesktopDisplayMode(0, &mode));
    return mode.w > mode.h;
  }

 private:
#if 0
  LRESULT OnWndProc(UINT msg, WPARAM w_param, LPARAM l_param) {
    switch (msg) {
      case WM_ERASEBKGND:
        return 1;
      case WM_PAINT:
        Paint();
        return 0;
      case WM_SIZE:
        RECT rc;
        GetClientRect(hwnd_, &rc);
        Gpu::Resize(this, Rect(rc.left, rc.top,
                            rc.right - rc.left, rc.bottom - rc.top));
        return 0;
      case WM_CREATE:
        DCHECK(got_valid_hwnd_);
        Gpu::InitializeForRenderingSurface(this, hwnd_);
        return 0;
      case WM_CLOSE:
        // TODO(scottmg): Probably not where this should be.
        MessageLoopForUI::current()->QuitNow();
        return 0;

      case WM_MOUSEMOVE: {
        int x = static_cast<SHORT>(LOWORD(l_param));
        int y = static_cast<SHORT>(HIWORD(l_param));
        int dx = x - last_mouse_x_;
        int dy = y - last_mouse_y_;
        last_mouse_x_ = x;
        last_mouse_y_ = y;
        DCHECK(workspace_->WantMouseEvents());
        workspace_->NotifyMouseMoved(x, y, dx, dy, GetInputModifiers());
        break;
      }

      case WM_LBUTTONDOWN:
      case WM_RBUTTONDOWN:
      case WM_MBUTTONDOWN: {
        SetCapture(hwnd_);
        int index = msg == WM_LBUTTONDOWN ? 0 :
                    msg == WM_RBUTTONDOWN ? 1 :
                                            2;
        DCHECK(workspace_->WantMouseEvents());
        workspace_->NotifyMouseButton(index, true, GetInputModifiers());
        break;
      }

      case WM_LBUTTONUP:
      case WM_RBUTTONUP:
      case WM_MBUTTONUP: {
        ReleaseCapture();
        int index = msg == WM_LBUTTONUP ? 0 :
                    msg == WM_RBUTTONUP ? 1 :
                                          2;
        DCHECK(workspace_->WantMouseEvents());
        workspace_->NotifyMouseButton(index, false, GetInputModifiers());
        break;
      }

      case WM_MOUSEWHEEL:
        DCHECK(workspace_->WantMouseEvents());
        workspace_->NotifyMouseWheel(
            static_cast<SHORT>(HIWORD(w_param)),
            GetInputModifiers());
        break;

      case WM_SYSKEYDOWN:
      case WM_SYSKEYUP:
      case WM_KEYDOWN:
      case WM_KEYUP: {
        bool down = msg == WM_KEYDOWN || msg == WM_SYSKEYDOWN;
        InputKey key;
        switch (w_param) {
          case VK_SHIFT: key = kShift; break;
          case VK_RETURN: key = kReturn; break;
          case VK_BACK: key = kBackspace; break;
          case VK_DELETE: key = kDelete; break;
          case VK_TAB: key = kTab; break;
          case VK_SPACE: key = kSpace; break;
          case VK_HOME: key = kHome; break;
          case VK_END: key = kEnd; break;
          case VK_CONTROL: key = kControl; break;
          case VK_UP: key = kUp; break;
          case VK_DOWN: key = kDown; break;
          case VK_LEFT: key = kLeft; break;
          case VK_RIGHT: key = kRight; break;
          case VK_PRIOR: key = kPageUp; break;
          case VK_NEXT: key = kPageDown; break;
          case VK_F1: key = kF1; break;
          case VK_F2: key = kF2; break;
          case VK_F3: key = kF3; break;
          case VK_F4: key = kF4; break;
          case VK_F5: key = kF5; break;
          case VK_F6: key = kF6; break;
          case VK_F7: key = kF7; break;
          case VK_F8: key = kF8; break;
          case VK_F9: key = kF9; break;
          case VK_F10: key = kF10; break;
          case VK_F11: key = kF11; break;
          case VK_F12: key = kF12; break;
          default: key = kNone; break;
        }

        if (key != kNone) {
          DCHECK(workspace_->WantKeyEvents());
          if (workspace_->NotifyKey(key, down, GetInputModifiers()))
            return 0;
        }
        break;
      }

      case WM_SETCURSOR: {
        RECT rect;
        // We want DefWindowProc to handle NC.
        if (GetClientRect(hwnd_, &rect)) {
          POINT pos;
          if (GetCursorPos(&pos)) {
            if (ScreenToClient(hwnd_, &pos)) {
              if (PtInRect(&rect, pos))
                return 0;
            }
          }
        }
      }
    }
    return DefWindowProc(hwnd_, msg, w_param, l_param);
  }

  InputModifiers GetInputModifiers() {
    return InputModifiers(
        GetKeyState(VK_LCONTROL) & 0x8000,
        GetKeyState(VK_RCONTROL) & 0x8000,
        GetKeyState(VK_LSHIFT) & 0x8000,
        GetKeyState(VK_RSHIFT) & 0x8000,
        GetKeyState(VK_LMENU) & 0x8000,
        GetKeyState(VK_RMENU) & 0x8000);
  }
#endif

  SDL_Window* window_;
  Workspace* workspace_;
  int last_mouse_x_;
  int last_mouse_y_;

  DISALLOW_COPY_AND_ASSIGN(ApplicationWindowSdl);
};

}  // namespace

ApplicationWindow* ApplicationWindow::Create() {
  return new ApplicationWindowSdl;
}
