// Copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "sg/application_window.h"

#include "base/basictypes.h"
#include "base/bind.h"
#include "sg/app_thread.h"
#include "sg/gpu.h"
#include "sg/ui/base_types.h"
#include "sg/ui/input.h"
#include "sg/workspace.h"

namespace {

void* SetWindowUserData(HWND hwnd, void* user_data) {
  return
      reinterpret_cast<void*>(SetWindowLongPtr(hwnd, GWLP_USERDATA,
          reinterpret_cast<LONG_PTR>(user_data)));
}

void* GetWindowUserData(HWND hwnd) {
  DWORD process_id = 0;
  GetWindowThreadProcessId(hwnd, &process_id);
  // A window outside the current process needs to be ignored.
  if (process_id != ::GetCurrentProcessId())
    return NULL;
  return reinterpret_cast<void*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
}

class ApplicationWindowWin : public ApplicationWindow {
 public:
  ApplicationWindowWin()
      : got_create_(false),
        got_valid_hwnd_(false),
        workspace_(NULL),
        last_mouse_x_(0),
        last_mouse_y_(0) {
    CreateHwnd();
  }

  virtual ~ApplicationWindowWin() {
  }

  virtual void Show() OVERRIDE {
    DCHECK(AppThread::CurrentlyOn(AppThread::UI));
    SetWindowText(hwnd_, L"Seaborgium");
    ShowWindow(hwnd_, SW_SHOWMAXIMIZED);
    SetForegroundWindow(hwnd_);
    SetFocus(hwnd_);
  }

  virtual void Paint() OVERRIDE {
    Gpu::Paint(this);
    ValidateRect(hwnd_, NULL);
  }

  virtual void SetDebugPresenterNotify(DebugPresenterNotify* notifier) {
    Gpu::SetDebugPresenterNotify(this, notifier);
  }

  virtual void SetContents(Workspace* workspace) {
    workspace_ = workspace;
  }
  virtual Workspace* GetContents() { return workspace_; }

  virtual bool IsLandscape() {
    // TODO(config): Probably wrong on multi mon, but should be removed anyway.
    int w = GetSystemMetrics(SM_CXFULLSCREEN);
    int h = GetSystemMetrics(SM_CYFULLSCREEN);
    return w > h;
  }

 private:
  LRESULT OnWndProc(UINT msg, WPARAM w_param, LPARAM l_param) {
    switch (msg) {
      case WM_ERASEBKGND:
        return 1;
      case WM_PAINT:
        /* TODO(gputhread)
        AppThread::PostTask(
            AppThread::GPU, FROM_HERE, base::Bind(&Gpu::Paint, this));
            */
        Paint();
        return 0;
      case WM_SIZE:
        RECT rc;
        GetClientRect(hwnd_, &rc);
        Gpu::Resize(this, Rect(rc.left, rc.top,
                            rc.right - rc.left, rc.bottom - rc.top));
        /* TODO(gputhread)
        AppThread::PostTask(
            AppThread::GPU, FROM_HERE,
            base::Bind(&Gpu::Resize,
                       this,
                       Rect(rc.left, rc.top,
                            rc.right - rc.left, rc.bottom - rc.top)));
                            */
        return 0;
      case WM_CREATE:
        DCHECK(got_valid_hwnd_);
        /* TODO(gputhread)
        AppThread::PostTask(
            AppThread::GPU,
            FROM_HERE,
            base::Bind(&Gpu::InitializeForRenderingSurface, this, hwnd_));
            */
        Gpu::InitializeForRenderingSurface(this, hwnd_);
        return 0;
      case WM_CLOSE:
        // TODO(scottmg): Probably not where this should be.
        MessageLoopForUI::current()->Quit();
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
          workspace_->NotifyKey(key, down, GetInputModifiers());
          return 0;
        }
        break;
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

  static LRESULT CALLBACK WndProc(
      HWND hwnd, UINT message, WPARAM w_param, LPARAM l_param) {
    if (message == WM_NCCREATE) {
      CREATESTRUCT* cs = reinterpret_cast<CREATESTRUCT*>(l_param);
      ApplicationWindowWin* window =
        reinterpret_cast<ApplicationWindowWin*>(cs->lpCreateParams);
      DCHECK(window);
      SetWindowUserData(hwnd, window);
      window->hwnd_ = hwnd;
      window->got_create_ = true;
      if (hwnd)
        window->got_valid_hwnd_ = true;
      return TRUE;
    }

    ApplicationWindowWin* window = reinterpret_cast<ApplicationWindowWin*>(
        GetWindowUserData(hwnd));
    if (!window)
      return 0;

    return window->OnWndProc(message, w_param, l_param);
  }

  void CreateHwnd() {
    WNDCLASS  wc;
    ZeroMemory(&wc, sizeof(wc));

    wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
    wc.lpfnWndProc = WndProc;
    wc.hInstance = GetModuleHandle(NULL);
    wc.lpszClassName = L"SeaborgiumWindow";
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = static_cast<HBRUSH>(GetStockObject(BLACK_BRUSH));
    wc.hIcon = LoadIcon(wc.hInstance, MAKEINTRESOURCE(1));
    RegisterClass(&wc);

    int width = 1280;
    int height = 1024;
    RECT rect;
    SystemParametersInfo(SPI_GETWORKAREA, 0, &rect, 0);
    // TODO(scottmg): The window text doesn't get set, for reasons unclear to
    // me. It's set at initial show above instead.
    CreateWindowExW(
        (WS_EX_APPWINDOW | WS_EX_WINDOWEDGE),
        wc.lpszClassName,
        L"",
        WS_OVERLAPPEDWINDOW,
        (rect.right - rect.left - width) / 2 + rect.left,
        (rect.bottom - rect.top - height) / 2 + rect.top,
        width, height,
        NULL, NULL, GetModuleHandle(NULL),
        reinterpret_cast<void*>(this));
  }

  HWND hwnd_;
  bool got_create_;
  bool got_valid_hwnd_;
  Workspace* workspace_;
  int last_mouse_x_;
  int last_mouse_y_;

  DISALLOW_COPY_AND_ASSIGN(ApplicationWindowWin);
};

}  // namespace

ApplicationWindow* ApplicationWindow::Create() {
  return new ApplicationWindowWin;
}
