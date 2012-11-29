#define WIN32_LEAN_AND_MEAN
#include <crtdbg.h>
#include <windows.h>

#include "Gwen/Gwen.h"
#include "Gwen/Input/Windows.h"
#include "Gwen/Renderers/DirectX9.h"
#include "Gwen/Skins/Simple.h"
#include "Gwen/Skins/TexturedBase.h"
#include "Gwen/UnitTest/UnitTest.h"
#include "lexer.h"
#include "re2/re2.h"
#include "top_level_frame.h"

#include <d3d9.h>
#include <d3dx9core.h>
#include <d3dx9math.h>
#include <dxerr.h>
#include <memory.h>
#include <stdlib.h>
#include <tchar.h>

HWND g_pHWND = NULL;
LPDIRECT3D9 g_pD3D = NULL;
IDirect3DDevice9* g_pD3DDevice = NULL;
D3DPRESENT_PARAMETERS g_D3DParams;

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


void ResetD3DDevice() {
  g_pD3DDevice->Reset(&g_D3DParams);
}

void CreateD3DDevice() {
  ZeroMemory(&g_D3DParams, sizeof(g_D3DParams));

  RECT ClientRect;
  GetClientRect(g_pHWND, &ClientRect);

  g_D3DParams.Windowed = TRUE;
  g_D3DParams.SwapEffect = D3DSWAPEFFECT_DISCARD;
  g_D3DParams.BackBufferWidth = ClientRect.right;
  g_D3DParams.BackBufferHeight = ClientRect.bottom;
  g_D3DParams.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;
  g_D3DParams.BackBufferFormat = D3DFMT_X8R8G8B8;
  //g_D3DParams.EnableAutoDepthStencil = TRUE;
  //g_D3DParams.AutoDepthStencilFormat = D3DFMT_D24S8;
  //g_D3DParams.PresentationInterval = D3DPRESENT_INTERVAL_DEFAULT;
  g_D3DParams.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;

  HRESULT hr = g_pD3D->CreateDevice(
      D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL,
      g_pHWND, D3DCREATE_HARDWARE_VERTEXPROCESSING,
      &g_D3DParams, &g_pD3DDevice);
  if (FAILED(hr)) {
    OutputDebugString(L"Couldn't create D3D Device for some reason!");
    return;
  }
}

int main(
    HINSTANCE hInstance,
    HINSTANCE hPrevInstance,
    LPTSTR lpCmdLine,
    int nCmdShow) {
  _CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_FILE | _CRTDBG_MODE_DEBUG);
  _CrtSetReportFile(_CRT_WARN, _CRTDBG_FILE_STDERR);
  _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

  g_pD3D = Direct3DCreate9(D3D_SDK_VERSION);
  g_pHWND = CreateMainWindow();
  CreateD3DDevice();
  RECT FrameBounds;
  GetClientRect(g_pHWND, &FrameBounds);

  Gwen::Renderer::DirectX9* renderer =
      new Gwen::Renderer::DirectX9(g_pD3DDevice);

  Gwen::Skin::TexturedBase* skin = new Gwen::Skin::TexturedBase(renderer);
  skin->Init("DefaultSkin.png");
  skin->SetDefaultFont(L"Consolas", 9.f);

  Gwen::Controls::Canvas* canvas = new Gwen::Controls::Canvas(skin);
  canvas->SetSize(FrameBounds.right, FrameBounds.bottom);
  canvas->SetDrawBackground(true);

  TopLevelFrame* top_level = new TopLevelFrame(canvas);
  top_level->SetPos(0, 0);

  Gwen::Input::Windows GwenInput;
  GwenInput.Initialize(canvas);

  MSG msg;
  while(true) {
    // Skip out if the window is closed
    if (!IsWindowVisible(g_pHWND))
      break;

    // If we have a message from windows..
    if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
    {
      // .. give it to the input handler to process
      GwenInput.ProcessMessage(msg);

      // if it's QUIT then quit..
      if (msg.message == WM_QUIT)
        break;

      // Handle the regular window stuff..
      TranslateMessage(&msg);
      DispatchMessage(&msg);
    }
    else {
      g_pD3DDevice->BeginScene();
      g_pD3DDevice->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(255, 255, 255), 1, 0);
      canvas->RenderCanvas();
      g_pD3DDevice->EndScene();
      g_pD3DDevice->Present(NULL, NULL, NULL, NULL);
    }
  }

  delete canvas;
  delete skin;
  delete renderer;

  if (g_pD3DDevice) {
    g_pD3DDevice->Release();
    g_pD3DDevice = NULL;
  }

  if (g_pD3D) {
    g_pD3D->Release();
    g_pD3D = NULL;
  }

#ifdef _DEBUG
  re2::TidyUpGlobals();
  Lexer::TidyUpGlobals();
#endif
}
