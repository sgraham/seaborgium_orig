#define WIN32_LEAN_AND_MEAN
#define _INTSAFE_H_INCLUDED_ // What a cluster.
#include <windows.h>
#include <d2d1.h>
#include <d2d1helper.h>
#include <dwrite.h>
#include <wincodec.h>

#include "base/logging.h"
#include "Gwen/Gwen.h"
#include "Gwen/Input/Windows.h"
#include "Gwen/Renderers/Direct2D.h"
#include "Gwen/Skins/Simple.h"
#include "Gwen/Skins/TexturedBase.h"
#include "lexer.h"
#include "re2/re2.h"
#include "top_level_frame.h"

#pragma comment(lib, "d2d1.lib")
#pragma comment(lib, "dwrite.lib")
#pragma comment(lib, "windowscodecs.lib")

HWND g_pHWND = NULL;
ID2D1Factory* g_pD2DFactory = NULL;
IDWriteFactory* g_pDWriteFactory = NULL;
IWICImagingFactory* g_pWICFactory = NULL;
ID2D1HwndRenderTarget* g_pRT = NULL; // this is device-specific

Gwen::Renderer::Direct2D* g_pRenderer = NULL;

HWND CreateMainWindow() {
  WNDCLASS  wc;
  ZeroMemory(&wc, sizeof(wc));

  wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
  wc.lpfnWndProc = DefWindowProc;
  wc.hInstance = GetModuleHandle(NULL);
  wc.lpszClassName = L"SeaborgiumWindow";
  wc.hCursor = LoadCursor(NULL, IDC_ARROW);

  RegisterClass(&wc);

  int width = 1280;
  int height = 1024;
  RECT rect;
  SystemParametersInfo(SPI_GETWORKAREA, 0, &rect, 0);
  HWND hWindow = CreateWindowExW(
      (WS_EX_APPWINDOW | WS_EX_WINDOWEDGE),
      wc.lpszClassName,
      L"Seaborgium",
      WS_OVERLAPPEDWINDOW,
      (rect.right - rect.left - width) / 2 + rect.left,
      (rect.bottom - rect.top - height) / 2 + rect.top,
      width, height,
      NULL, NULL, GetModuleHandle(NULL), NULL);

  ShowWindow(hWindow, SW_SHOW);
  SetForegroundWindow(hWindow);
  SetFocus(hWindow);

  return hWindow;
}

HRESULT CreateDeviceResources() {
  HRESULT hr = S_OK;
  if (!g_pRT) {
    RECT rc;
    GetClientRect(g_pHWND, &rc);

    D2D1_SIZE_U size = D2D1::SizeU(
        rc.right - rc.left,
        rc.bottom - rc.top);

    // Create a Direct2D render target.
    hr = g_pD2DFactory->CreateHwndRenderTarget(
        D2D1::RenderTargetProperties(),
        D2D1::HwndRenderTargetProperties(g_pHWND, size),
        &g_pRT);

    if (SUCCEEDED(hr) && g_pRenderer != NULL) {
      g_pRT->SetTextAntialiasMode(D2D1_TEXT_ANTIALIAS_MODE_CLEARTYPE);
      g_pRenderer->DeviceAcquired(g_pRT);
    }
  }
  return hr;
}

void discardDeviceResources() {
  if (g_pRT != NULL) {
    g_pRT->Release();
    g_pRT = NULL;
  }
}

void RunMain() {
  RECT FrameBounds;
  GetClientRect(g_pHWND, &FrameBounds);

  Gwen::Skin::TexturedBase skin(g_pRenderer);
  skin.Init("DefaultSkin.png");
  skin.SetDefaultFont(L"Consolas", 12.f);

  Gwen::Controls::Canvas* canvas = new Gwen::Controls::Canvas(&skin);
  canvas->SetSize(FrameBounds.right, FrameBounds.bottom);
  canvas->SetDrawBackground(true);

  TopLevelFrame* top_level = new TopLevelFrame(canvas);
  top_level->SetPos(0, 0);

  Gwen::Input::Windows GwenInput;
  GwenInput.Initialize(canvas);

  MSG msg;
  while(true) {
    if (!IsWindowVisible(g_pHWND))
      break;

    if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
      GwenInput.ProcessMessage(msg);
      if (msg.message == WM_QUIT)
        break;
      TranslateMessage(&msg);
      DispatchMessage(&msg);
    }

    {
      if (SUCCEEDED(CreateDeviceResources())) {
        g_pRT->BeginDraw();
        g_pRT->SetTransform(D2D1::Matrix3x2F::Identity());
        g_pRT->Clear(D2D1::ColorF(D2D1::ColorF::White));
        canvas->RenderCanvas();
        HRESULT hr = g_pRT->EndDraw();
        if (hr == D2DERR_RECREATE_TARGET) {
          discardDeviceResources();
          g_pRenderer->DeviceLost();
        }
      }
    }
  }

  delete canvas;
}

int main(
    HINSTANCE hInstance,
    HINSTANCE hPrevInstance,
    LPTSTR lpCmdLine,
    int nCmdShow) {
  _CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_FILE | _CRTDBG_MODE_DEBUG);
  _CrtSetReportFile(_CRT_WARN, _CRTDBG_FILE_STDERR);
  _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

  HRESULT hr = D2D1CreateFactory(
    D2D1_FACTORY_TYPE_SINGLE_THREADED,
    &g_pD2DFactory);
  DCHECK(SUCCEEDED(hr));

  hr = DWriteCreateFactory(
    DWRITE_FACTORY_TYPE_SHARED,
    __uuidof(IDWriteFactory),
    reinterpret_cast<IUnknown**>(&g_pDWriteFactory));
  DCHECK(SUCCEEDED(hr));

  hr = CoInitialize(NULL);
  DCHECK(SUCCEEDED(hr));

  hr = CoCreateInstance(
    CLSID_WICImagingFactory,
    NULL,
    CLSCTX_INPROC_SERVER,
    IID_IWICImagingFactory,
    reinterpret_cast<void **>(&g_pWICFactory));
  DCHECK(SUCCEEDED(hr));

  g_pHWND = CreateMainWindow();
  CreateDeviceResources();
  g_pRenderer = new Gwen::Renderer::Direct2D(
      g_pRT, g_pDWriteFactory, g_pWICFactory);

  RunMain();

  delete g_pRenderer;
  g_pRenderer = NULL;

  if(g_pRT != NULL)
  {
    g_pRT->Release();
    g_pRT = NULL;
  }

#ifdef _DEBUG
  re2::TidyUpGlobals();
  Lexer::TidyUpGlobals();
#endif
}

