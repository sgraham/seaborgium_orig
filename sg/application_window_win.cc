// Copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "sg/application_window.h"

#include "base/basictypes.h"
#include "base/bind.h"
#include "sg/app_thread.h"
#include "sg/gpu.h"
#include "sg/ui/base_types.h"

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
  ApplicationWindowWin() : got_create_(false), got_valid_hwnd_(false) {
    CreateHwnd();
  }

  virtual ~ApplicationWindowWin() {
  }

  virtual void Show() OVERRIDE {
    DCHECK(AppThread::CurrentlyOn(AppThread::UI));
    ShowWindow(hwnd_, SW_SHOWMAXIMIZED);
    SetForegroundWindow(hwnd_);
    SetFocus(hwnd_);
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
        Gpu::Paint(this);
        ValidateRect(hwnd_, NULL);
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
    }
    return DefWindowProc(hwnd_, msg, w_param, l_param);
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
    RegisterClass(&wc);

    int width = 1280;
    int height = 1024;
    RECT rect;
    SystemParametersInfo(SPI_GETWORKAREA, 0, &rect, 0);
    CreateWindowExW(
        (WS_EX_APPWINDOW | WS_EX_WINDOWEDGE),
        wc.lpszClassName,
        L"Seaborgium",
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

  DISALLOW_COPY_AND_ASSIGN(ApplicationWindowWin);
};

}  // namespace

ApplicationWindow* ApplicationWindow::Create() {
  return new ApplicationWindowWin;
}
