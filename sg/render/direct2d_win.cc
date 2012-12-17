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

#include <windows.h>
#include <d2d1.h>
#include <d2d1helper.h>
#include <dwrite.h>
#include <wincodec.h>

#include "base/logging.h"
#include "sg/render/direct2d_win.h"
#include "sg/render/font.h"
#include "sg/render/texture.h"

struct FontData {
  IDWriteTextFormat* text_format;
};

struct TextureData {
  ID2D1Bitmap* bitmap;  // device-specific
  IWICBitmapSource* wic_bitmap;
};


Direct2DRenderer::Direct2DRenderer(
    ID2D1RenderTarget* rt,
    IDWriteFactory* dwrite_factory,
    IWICImagingFactory* wic_factory)
    : dwrite_factory_(dwrite_factory),
      wic_factory_(wic_factory),
      color_(D2D1::ColorF::White) {
  DeviceAcquired(rt);
}

Direct2DRenderer::~Direct2DRenderer() {
}

void Direct2DRenderer::DrawFilledRect(Rect rect) {
  TranslateByRenderOffset(&rect);
  if (solid_color_brush_)
    rt_->FillRectangle(
        D2D1::RectF(rect.x, rect.y, rect.x + rect.w, rect.y + rect.h),
        solid_color_brush_);
}

void Direct2DRenderer::SetDrawColor(Color color) {
  color_ = D2D1::ColorF(
      color.r / 255.0f, color.g / 255.0f, color.b / 255.0f, color.a / 255.0f);
  solid_color_brush_->SetColor(color_);
}

bool Direct2DRenderer::InternalLoadFont(Font* font) {
  IDWriteTextFormat* text_format = NULL;

  HRESULT hr = dwrite_factory_->CreateTextFormat(
      font->facename.c_str(),
      NULL,
      font->bold ? DWRITE_FONT_WEIGHT_BOLD : DWRITE_FONT_WEIGHT_NORMAL,
      DWRITE_FONT_STYLE_NORMAL,
      DWRITE_FONT_STRETCH_NORMAL,
      font->size,
      L"",
      &text_format);

  if (SUCCEEDED(hr)) {
    FontData* font_data = new FontData();
    font_data->text_format = text_format;
    font->data = font_data;
    return true;
  }

  return false;
}

void Direct2DRenderer::LoadFont(Font* font) {
  if (InternalLoadFont(font))
    font_list_.push_back(font);
}

void Direct2DRenderer::InternalFreeFont(Font* font, bool bRemove) {
  if (bRemove)
    font_list_.remove(font);

  if (!font->data)
    return;

  FontData* font_data = reinterpret_cast<FontData*>(font->data);
  font_data->text_format->Release();
  delete font_data;
  font->data = NULL;
}

void Direct2DRenderer::FreeFont(Font* font) {
  InternalFreeFont(font);
}

void Direct2DRenderer::RenderText(Font* font, Point pos, const string16& text) {
  // If the font doesn't exist, or the font size should be changed
  if (!font->data) {
    InternalFreeFont(font, false);
    InternalLoadFont(font);
  }

  FontData* font_data = reinterpret_cast<FontData*>(font->data);

  TranslateByRenderOffset(&pos.x, &pos.y);

  if (solid_color_brush_) {
    rt_->DrawTextW(
        text.c_str(),
        text.length(),
        font_data->text_format,
        D2D1::RectF(pos.x, pos.y, pos.x + 50000, pos.y + 50000),
        solid_color_brush_);
  }
}

Point Direct2DRenderer::MeasureText(Font* font, const string16& text) {
  // If the font doesn't exist.
  if (!font->data) {
    InternalFreeFont(font, false);
    InternalLoadFont(font);
  }

  FontData* font_data = reinterpret_cast<FontData*>(font->data);

  Point size;
  IDWriteTextLayout* pLayout;
  DWRITE_TEXT_METRICS metrics;

  dwrite_factory_->CreateTextLayout(
      text.c_str(),
      text.length(),
      font_data->text_format,
      50000,
      50000,
      &pLayout);

  pLayout->GetMetrics(&metrics);

  pLayout->Release();

  return Point(metrics.widthIncludingTrailingWhitespace, metrics.height);
}

void Direct2DRenderer::DeviceLost() {
  if (solid_color_brush_ != NULL) {
    solid_color_brush_->Release();
    solid_color_brush_ = NULL;
  }

  for (std::list<Texture*>::const_iterator tex_it = texture_list_.begin();
       tex_it != texture_list_.end(); ++tex_it) {
    InternalFreeTexture(*tex_it, false);
  }
}

void Direct2DRenderer::DeviceAcquired(ID2D1RenderTarget* rt) {
  rt_ = rt;

  HRESULT hr = rt_->CreateSolidColorBrush(color_, &solid_color_brush_);
  (void)hr;

  for (std::list<Texture*>::const_iterator tex_it = texture_list_.begin();
       tex_it != texture_list_.end(); ++tex_it) {
    InternalLoadTexture(*tex_it);
  }
}

void Direct2DRenderer::StartClip() {
  Rect rect = ClipRegion();

  D2D1_RECT_F r =
      D2D1::RectF(rect.x, rect.y, rect.x + rect.w, rect.y + rect.h);

  rt_->PushAxisAlignedClip(r, D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);
}

void Direct2DRenderer::EndClip() {
  rt_->PopAxisAlignedClip();
}

void Direct2DRenderer::DrawTexturedRectAlpha(
    Texture* texture,
    Rect rect,
    float alpha,
    float u1, float v1, float u2, float v2) {
  TextureData* tex_data = reinterpret_cast<TextureData*>(texture->data);

  // Missing image, not loaded properly?
  if (!texture || tex_data->bitmap == NULL)
    return DrawMissingImage(rect);

  TranslateByRenderOffset(&rect);

  rt_->DrawBitmap(tex_data->bitmap,
      D2D1::RectF(rect.x, rect.y, rect.x + rect.w, rect.y + rect.h),
      alpha,
      D2D1_BITMAP_INTERPOLATION_MODE_LINEAR,
      D2D1::RectF(u1 * texture->width,
                  v1 * texture->height,
                  u2 * texture->width,
                  v2 * texture->height));
}

bool Direct2DRenderer::InternalLoadTexture(Texture* texture) {
  IWICBitmapDecoder* pDecoder = NULL;
  IWICBitmapFrameDecode* pSource = NULL;
  IWICFormatConverter* pConverter = NULL;
  ID2D1Bitmap* pD2DBitmap = NULL;

  HRESULT hr = wic_factory_->CreateDecoderFromFilename(
      texture->name.c_str(),
      NULL,
      GENERIC_READ,
      WICDecodeMetadataCacheOnLoad,
      &pDecoder);

  if (SUCCEEDED(hr))
    hr = pDecoder->GetFrame(0, &pSource);

  if (SUCCEEDED(hr)) {
    // Convert the image format to 32bppPBGRA
    // (DXGI_FORMAT_B8G8R8A8_UNORM + D2D1_ALPHA_MODE_PREMULTIPLIED).
    hr = wic_factory_->CreateFormatConverter(&pConverter);
  }

  if (SUCCEEDED(hr)) {
    hr = pConverter->Initialize(
        pSource,
        GUID_WICPixelFormat32bppPBGRA,
        WICBitmapDitherTypeNone,
        NULL,
        0.f,
        WICBitmapPaletteTypeMedianCut);
  }

  if (SUCCEEDED(hr))
    hr = rt_->CreateBitmapFromWicBitmap(pConverter, NULL, &pD2DBitmap);

  if (SUCCEEDED(hr)) {
    TextureData* texdata = new TextureData();

    texdata->wic_bitmap = pSource;
    texdata->bitmap = pD2DBitmap;

    texture->data = texdata;

    D2D1_SIZE_F size = texdata->bitmap->GetSize();

    texture->width = size.width;
    texture->height = size.height;
    texture->failed = false;
  } else {
    texture->failed = true;
  }

  if (pDecoder != NULL)
    pDecoder->Release();

  if (pConverter != NULL)
    pConverter->Release();

  return SUCCEEDED(hr);
}

void Direct2DRenderer::LoadTexture(Texture* texture) {
  if (InternalLoadTexture(texture))
    texture_list_.push_back(texture);
}

void Direct2DRenderer::InternalFreeTexture(Texture* texture, bool bRemove) {
  if (bRemove)
    texture_list_.remove(texture);

  if (texture->data != NULL) {
    TextureData* tex_data = reinterpret_cast<TextureData*>(texture->data);
    if (tex_data->wic_bitmap != NULL)
      tex_data->wic_bitmap->Release();
    if (tex_data->bitmap != NULL)
      tex_data->bitmap->Release();
    delete tex_data;
  }

  texture->data = NULL;
}

void Direct2DRenderer::FreeTexture(Texture* texture) {
  InternalFreeTexture(texture);
}

void Direct2DRenderer::Release() {
  while (!texture_list_.empty()) {
    FreeTexture(texture_list_.front());
    texture_list_.pop_front();
  }

  while (!font_list_.empty()) {
    FreeFont(font_list_.front());
    font_list_.pop_front();
  }
}

void Direct2DRenderer::DrawRenderToTextureResult(
    RenderToTextureRenderer* renderer,
    Rect rect,
    float alpha,
    float u1, float v1, float u2, float v2) {
  TranslateByRenderOffset(&rect);

  Direct2DRenderToTextureRenderer* as_d2d =
      reinterpret_cast<Direct2DRenderToTextureRenderer*>(renderer);
  ID2D1Bitmap* bitmap = as_d2d->FinishAndGetBitmap();

  rt_->DrawBitmap(bitmap,
      D2D1::RectF(rect.x, rect.y, rect.x + rect.w, rect.y + rect.h),
      alpha,
      D2D1_BITMAP_INTERPOLATION_MODE_LINEAR,
      D2D1::RectF(u1 * as_d2d->width(),
                  v1 * as_d2d->height(),
                  u2 * as_d2d->width(),
                  v2 * as_d2d->height()));
}

Direct2DRenderToTextureRenderer*
    Direct2DRenderer::CreateRenderToTextureRenderer(int width, int height) {
  return new Direct2DRenderToTextureRenderer(this, width, height);
}

Direct2DRenderToTextureRenderer::Direct2DRenderToTextureRenderer(
    Direct2DRenderer* main_renderer,
    int width,
    int height)
    : main_renderer_(main_renderer),
      color_(D2D1::ColorF::White),
      width_(width),
      height_(height) {
  HRESULT hr = main_renderer_->render_target()->CreateCompatibleRenderTarget(
      D2D1::SizeF(width, height),
      &rt_);
  CHECK(SUCCEEDED(hr));
  rt_->BeginDraw();

  hr = rt_->CreateSolidColorBrush(color_, &solid_color_brush_);
  CHECK(SUCCEEDED(hr));
}

Direct2DRenderToTextureRenderer::~Direct2DRenderToTextureRenderer() {
}

void Direct2DRenderToTextureRenderer::StartClip() {
  NOTIMPLEMENTED();
}

void Direct2DRenderToTextureRenderer::EndClip() {
  NOTIMPLEMENTED();
}

void Direct2DRenderToTextureRenderer::SetDrawColor(Color color) {
  color_ = D2D1::ColorF(
      color.r / 255.0f, color.g / 255.0f, color.b / 255.0f, color.a / 255.0f);
  solid_color_brush_->SetColor(color_);
}

void Direct2DRenderToTextureRenderer::DrawFilledRect(Rect rect) {
  TranslateByRenderOffset(&rect);
  if (solid_color_brush_)
    rt_->FillRectangle(
        D2D1::RectF(rect.x, rect.y, rect.x + rect.w, rect.y + rect.h),
        solid_color_brush_);
}

void Direct2DRenderToTextureRenderer::LoadTexture(Texture* texture) {
  main_renderer_->LoadTexture(texture);
}

void Direct2DRenderToTextureRenderer::FreeTexture(Texture* texture) {
  main_renderer_->FreeTexture(texture);
}

void Direct2DRenderToTextureRenderer::DrawTexturedRectAlpha(
    Texture* texture,
    Rect target_rect,
    float alpha,
    float u1, float v1, float u2, float v2) {
  NOTIMPLEMENTED();
}

void Direct2DRenderToTextureRenderer::DrawRenderToTextureResult(
    RenderToTextureRenderer* renderer,
    Rect target_rect,
    float alpha,
    float u1, float v1, float u2, float v2) {
  NOTIMPLEMENTED();
}

void Direct2DRenderToTextureRenderer::LoadFont(Font* font) {
  main_renderer_->LoadFont(font);
}

void Direct2DRenderToTextureRenderer::FreeFont(Font* font) {
  main_renderer_->FreeFont(font);
}

void Direct2DRenderToTextureRenderer::RenderText(
    Font* font, Point pos, const string16& text) {
  FontData* font_data = reinterpret_cast<FontData*>(font->data);
  TranslateByRenderOffset(&pos.x, &pos.y);
  if (solid_color_brush_) {
    rt_->DrawTextW(
        text.c_str(),
        text.length(),
        font_data->text_format,
        D2D1::RectF(pos.x, pos.y, pos.x + 50000, pos.y + 50000),
        solid_color_brush_);
  }
}

Point Direct2DRenderToTextureRenderer::MeasureText(
    Font* font, const string16& text) {
  return main_renderer_->MeasureText(font, text);
}

RenderToTextureRenderer*
    Direct2DRenderToTextureRenderer::CreateRenderToTextureRenderer(
        int width, int height) {
  return main_renderer_->CreateRenderToTextureRenderer(width, height);
}

ID2D1Bitmap* Direct2DRenderToTextureRenderer::FinishAndGetBitmap() {
  rt_->EndDraw();
  ID2D1Bitmap* bitmap;
  HRESULT hr = rt_->GetBitmap(&bitmap);
  CHECK(SUCCEEDED(hr));
  return bitmap;
}
