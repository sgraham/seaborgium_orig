// Copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "sg/gpu.h"

#define WIN32_LEAN_AND_MEAN
#define _INTSAFE_H_INCLUDED_  // What a cluster.
#include <windows.h>
#include <d2d1.h>
#include <d2d1helper.h>
#include <dwrite.h>
#include <map>
#include <wincodec.h>

#include "base/bind.h"
#include "base/lazy_instance.h"
#include "sg/app_thread.h"
#include "sg/application_window.h"

#pragma comment(lib, "d2d1.lib")
#pragma comment(lib, "dwrite.lib")
#pragma comment(lib, "windowscodecs.lib")

// Implementation of Gpu for Windows using Direct2D and DirectWrite.

namespace {

class Renderer {
 public:
  Renderer(HWND hwnd)
      : hwnd_(hwnd),
        d2d_factory_(NULL),
        dwrite_factory_(NULL),
        wic_factory_(NULL),
        render_target_(NULL) {
  }

  void Init() {
    HRESULT hr = D2D1CreateFactory(
      D2D1_FACTORY_TYPE_SINGLE_THREADED,
      &d2d_factory_);
    CHECK(SUCCEEDED(hr));

    hr = DWriteCreateFactory(
      DWRITE_FACTORY_TYPE_SHARED,
      __uuidof(IDWriteFactory),
      reinterpret_cast<IUnknown**>(&dwrite_factory_));
    CHECK(SUCCEEDED(hr));

    hr = CoCreateInstance(
      CLSID_WICImagingFactory,
      NULL,
      CLSCTX_INPROC_SERVER,
      IID_IWICImagingFactory,
      reinterpret_cast<void **>(&wic_factory_));
    CHECK(SUCCEEDED(hr));

    CreateDeviceResources();
  }

  void Paint() {
    if (SUCCEEDED(CreateDeviceResources())) {
      render_target_->BeginDraw();
      render_target_->SetTransform(D2D1::Matrix3x2F::Identity());
      // TODO(rendering)
      //g_main_canvas->RenderCanvas();
      HRESULT hr = render_target_->EndDraw();
      if (hr == D2DERR_RECREATE_TARGET) {
        DiscardDeviceResources();
        // TODO(rendering)
        //g_pRenderer->DeviceLost();
      }
    }
  }

 private:
  HRESULT CreateDeviceResources() {
    HRESULT hr = S_OK;
    if (!render_target_) {
      RECT rc;
      GetClientRect(hwnd_, &rc);

      D2D1_SIZE_U size = D2D1::SizeU(
          rc.right - rc.left,
          rc.bottom - rc.top);

      // Create a Direct2D render target.
      hr = d2d_factory_->CreateHwndRenderTarget(
          D2D1::RenderTargetProperties(),
          D2D1::HwndRenderTargetProperties(hwnd_, size),
          &render_target_);
      if (FAILED(hr))
        return hr;

      render_target_->SetTextAntialiasMode(D2D1_TEXT_ANTIALIAS_MODE_CLEARTYPE);

      IDWriteRenderingParams* default_params;
      IDWriteRenderingParams* custom_params;
      hr = dwrite_factory_->CreateRenderingParams(&default_params);
      if (FAILED(hr))
        return hr;
      hr = dwrite_factory_->CreateCustomRenderingParams(
          default_params->GetGamma(),
          default_params->GetEnhancedContrast(),
          default_params->GetClearTypeLevel(),
          default_params->GetPixelGeometry(),
          DWRITE_RENDERING_MODE_GDI_NATURAL,
          &custom_params);
      if (FAILED(hr))
        return hr;
      render_target_->SetTextRenderingParams(custom_params);

      // if (SUCCEEDED(hr) && g_pRenderer != NULL) {
        // TODO(rendering)
        //g_pRenderer->DeviceAcquired(render_target_);
      //}
    }
    return hr;
  }

  void DiscardDeviceResources() {
    if (render_target_) {
      render_target_->Release();
      render_target_ = NULL;
    }
  }

  HWND hwnd_;
  ID2D1Factory* d2d_factory_;
  IDWriteFactory* dwrite_factory_;
  IWICImagingFactory* wic_factory_;
  ID2D1HwndRenderTarget* render_target_;

  DISALLOW_COPY_AND_ASSIGN(Renderer);
};

base::LazyInstance<std::map<ApplicationWindow*, Renderer*> > g_window_map =
    LAZY_INSTANCE_INITIALIZER;

}  // namespace

// static
void Gpu::Paint(ApplicationWindow* window) {
  DCHECK(g_window_map.Get().find(window) != g_window_map.Get().end());
  g_window_map.Get()[window]->Paint();
}

// static
void Gpu::InitializeForRenderingSurface(
    ApplicationWindow* window, HWND hwnd) {
  OneTimeInitialization();

  Renderer* renderer = new Renderer(hwnd);
  DCHECK(g_window_map.Get().find(window) == g_window_map.Get().end());
  g_window_map.Get()[window] = renderer;
  renderer->Init();

  // This Unretained is OK because the ApplicationWindow isn't destroyed until
  // after all the other non-UI threads (including this one) are shut down.
  AppThread::PostTask(AppThread::UI, FROM_HERE, base::Bind(
      &ApplicationWindow::Show, base::Unretained(window)));
}

// static
void Gpu::OneTimeInitialization() {
  // TODO(rendering) We should have this at a thread initialization level
  // instead. HACK HACK
  static bool initialized = false;
  if (!initialized) {
    HRESULT hr = CoInitialize(NULL);
    CHECK(SUCCEEDED(hr));
  }
}

// TODO(scottmg): Thread shutdown for closing down these resources.
