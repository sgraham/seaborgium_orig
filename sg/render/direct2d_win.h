// Copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// Originally derived from some rendering code in "GWEN", copyright below:
/*
  GWEN

  Copyright (c) 2010 Facepunch Studios

  MIT License

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in
  all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
  THE SOFTWARE.
*/

#ifndef SG_RENDER_DIRECT2D_WIN_H_
#define SG_RENDER_DIRECT2D_WIN_H_

#include <d2d1.h>
#include <d2d1helper.h>
#include <dwrite.h>
#include <wincodec.h>
#include <list>

#include "base/compiler_specific.h"
#include "sg/base/string16.h"
#include "sg/basic_geometric_types.h"
#include "sg/render/renderer.h"

class Direct2DRenderer;
class Font;
class Texture;

class Direct2DRenderToTextureRenderer : public RenderToTextureRenderer {
 public:
  Direct2DRenderToTextureRenderer(
      Direct2DRenderer* main_renderer, int width, int height);
  virtual ~Direct2DRenderToTextureRenderer();

  virtual void StartClip() OVERRIDE;
  virtual void EndClip() OVERRIDE;

  virtual void SetDrawColor(Color color) OVERRIDE;

  virtual void DrawFilledRect(Rect rect) OVERRIDE;

  virtual void LoadTexture(Texture* texture) OVERRIDE;
  virtual void FreeTexture(Texture* texture) OVERRIDE;
  virtual void DrawTexturedRectAlpha(
      const Texture* texture,
      Rect target_rect,
      float alpha,
      float u1, float v1, float u2, float v2) OVERRIDE;

  virtual void DrawRenderToTextureResult(
      RenderToTextureRenderer* renderer,
      Rect target_rect,
      float alpha,
      float u1, float v1, float u2, float v2) OVERRIDE;

  virtual void LoadFont(Font* font) OVERRIDE;
  virtual void FreeFont(Font* font) OVERRIDE;
  virtual void RenderText(
      const Font* font, Point pos, const string16& text) OVERRIDE;
  virtual Point MeasureText(const Font* font, const string16& text) OVERRIDE;

  virtual RenderToTextureRenderer* CreateRenderToTextureRenderer(
      int width, int height) OVERRIDE;

 private:
  friend class Direct2DRenderer;

  ID2D1Bitmap* FinishAndGetBitmap();
  int width() const { return width_; }
  int height() const { return height_; }

  ID2D1SolidColorBrush* solid_color_brush_;
  D2D1::ColorF color_;

  Direct2DRenderer* main_renderer_;
  ID2D1BitmapRenderTarget* rt_;
  int width_;
  int height_;
};

class Direct2DRenderer : public Renderer {
 public:
  Direct2DRenderer(ID2D1RenderTarget* device,
                   IDWriteFactory* dwrite_factory,
                   IWICImagingFactory* wic_factory);
  ~Direct2DRenderer();

  virtual void Release();

  virtual void SetDrawColor(Color color) OVERRIDE;

  virtual void DrawFilledRect(Rect rect) OVERRIDE;

  virtual void LoadFont(Font* font) OVERRIDE;
  virtual void FreeFont(Font* font) OVERRIDE;
  virtual void RenderText(
      const Font* font, Point pos, const string16& text) OVERRIDE;
  virtual Point MeasureText(const Font* font, const string16& text) OVERRIDE;

  virtual void DeviceLost();
  virtual void DeviceAcquired(ID2D1RenderTarget* rt);

  void StartClip() OVERRIDE;
  void EndClip() OVERRIDE;

  void DrawTexturedRectAlpha(
      const Texture* texture,
      Rect target_rect,
      float alpha,
      float u1, float v1, float u2, float v2) OVERRIDE;
  void LoadTexture(Texture* texture) OVERRIDE;
  void FreeTexture(Texture* texture) OVERRIDE;

  virtual void DrawRenderToTextureResult(
      RenderToTextureRenderer* renderer,
      Rect target_rect,
      float alpha,
      float u1, float v1, float u2, float v2) OVERRIDE;

  virtual Direct2DRenderToTextureRenderer* CreateRenderToTextureRenderer(
      int width, int height) OVERRIDE;

 private:
  friend class Direct2DRenderToTextureRenderer;

  bool InternalLoadTexture(const Texture* texture);
  bool InternalLoadFont(const Font* texture);

  void InternalFreeFont(const Font* texture, bool remove = true);
  void InternalFreeTexture(const Texture* texture, bool remove = true);

  ID2D1RenderTarget* render_target() { return rt_; }

  IDWriteFactory* dwrite_factory_;
  IWICImagingFactory* wic_factory_;
  ID2D1RenderTarget* rt_;
  ID2D1Factory* d2d_factory_;
  HWND hwnd_;

  ID2D1SolidColorBrush* solid_color_brush_;
  D2D1::ColorF color_;

  std::list<Texture*> texture_list_;
  std::list<Font*> font_list_;
};

#endif  // SG_RENDER_DIRECT2D_WIN_H_
