#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include "Gwen/Gwen.h"
#include "Gwen/Input/Windows.h"
#include "Gwen/Skins/Simple.h"
#include "Gwen/Skins/TexturedBase.h"
#include "top_level_frame.h"

#include "Gwen/Renderers/OpenGL_DebugFont.h"
#include "gl/glew.h"

HWND g_pHWND = NULL;
Gwen::Controls::Canvas* main_canvas = NULL;

void Render() {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  if (main_canvas)
    main_canvas->RenderCanvas();
  SwapBuffers(GetDC(g_pHWND));
}

void OnSizeChanged() {
  RECT r;
  if (GetClientRect(g_pHWND, &r)) {
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    if (main_canvas)
      main_canvas->SetSize(r.right, r.bottom);
    glOrtho(r.left, r.right, r.bottom, r.top, -1.0, 1.0);
    glMatrixMode(GL_MODELVIEW);
    glViewport(0, 0, r.right - r.left, r.bottom - r.top);
  }
  Render();
}

LRESULT WINAPI WndProc(
    HWND hwnd, UINT msg, WPARAM w_param, LPARAM l_param) {
  switch (msg) {
    case WM_SIZE:
    case WM_SIZING:
      OnSizeChanged();
      return 0;
  }
  return DefWindowProc(hwnd, msg, w_param, l_param);
}

HWND CreateGameWindow(void) {
  WNDCLASSW wc;
  ZeroMemory(&wc, sizeof(wc));

  wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
  wc.lpfnWndProc = WndProc;
  wc.hInstance = GetModuleHandle(NULL);
  wc.lpszClassName = L"GWENWindow";
  wc.hCursor = LoadCursor(NULL, IDC_ARROW);

  RegisterClassW(&wc);

  HWND hWindow = CreateWindowExW(
      (WS_EX_APPWINDOW | WS_EX_WINDOWEDGE),
      wc.lpszClassName,
      L"Seaborgium",
      WS_OVERLAPPEDWINDOW,
      0, 0,
      1280, 1024,
      NULL, NULL, GetModuleHandle(NULL), NULL);

  ShowWindow(hWindow, SW_SHOW);
  SetForegroundWindow(hWindow);
  SetFocus(hWindow);

  return hWindow;
}

HGLRC CreateOpenGLDeviceContext() {
  PIXELFORMATDESCRIPTOR pfd = { 0 };
  pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
  pfd.nVersion = 1;

  pfd.dwFlags = PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER | PFD_DRAW_TO_WINDOW;

  pfd.iPixelType = PFD_TYPE_RGBA;
  pfd.cColorBits = 24;
  pfd.cDepthBits = 32;

  int pixelFormat = ChoosePixelFormat(GetDC(g_pHWND), &pfd);

  if (pixelFormat == 0)
    FatalAppExit(NULL, TEXT("ChoosePixelFormat() failed!"));

  SetPixelFormat(GetDC(g_pHWND), pixelFormat, &pfd);

  HGLRC opengl_context = wglCreateContext(GetDC(g_pHWND));
  wglMakeCurrent(GetDC(g_pHWND), opengl_context);

  OnSizeChanged();

  return opengl_context;
}


int main() {
  g_pHWND = CreateGameWindow();
  HGLRC opengl_context = CreateOpenGLDeviceContext();

  Gwen::Renderer::OpenGL *renderer = new Gwen::Renderer::OpenGL_DebugFont();

  renderer->Init();

  Gwen::Skin::TexturedBase* skin = new Gwen::Skin::TexturedBase(renderer);
  skin->Init("DefaultSkin.png");

  Gwen::Controls::Canvas* canvas = new Gwen::Controls::Canvas(skin);
  main_canvas = canvas;
  OnSizeChanged();
  canvas->SetDrawBackground(true);
  canvas->SetBackgroundColor(Gwen::Color(150, 170, 170, 255));

  TopLevelFrame* top_level = new TopLevelFrame(canvas);
  top_level->SetPos(0, 0);

  Gwen::Input::Windows gwen_input;
  gwen_input.Initialize(canvas);

  MSG msg;
  while(true) {
    if (!IsWindowVisible(g_pHWND))
      break;

    while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
      gwen_input.ProcessMessage(msg);
      if (msg.message == WM_QUIT)
        break;
      TranslateMessage(&msg);
      DispatchMessage(&msg);
    }

    Render();
  }

  wglMakeCurrent(NULL, NULL);
  wglDeleteContext(opengl_context);
  delete canvas;
  delete skin;
  delete renderer;
}

