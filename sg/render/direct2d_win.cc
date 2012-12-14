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

#include "sg/render/direct2d_win.h"
#include "sg/render/font.h"
#include "sg/render/texture.h"

struct FontData {
  IDWriteTextFormat*pTextFormat;
};

struct TextureData {
  ID2D1Bitmap* pBitmap;  // device-specific
  IWICBitmapSource* pWICBitmap;
};


Direct2DRenderer::Direct2DRenderer(
    ID2D1RenderTarget* pRT,
    IDWriteFactory* pDWriteFactory,
    IWICImagingFactory* pWICFactory)
    : m_Color(D2D1::ColorF::White) {
  DeviceAcquired(pRT);

  m_pDWriteFactory = pDWriteFactory;
  m_pWICFactory = pWICFactory;
}

Direct2DRenderer::~Direct2DRenderer() {
}

void Direct2DRenderer::DrawFilledRect(Rect rect) {
  TranslateByRenderOffset(&rect);
  if (solid_color_brush_)
    m_pRT->FillRectangle(D2D1::RectF(rect.x, rect.y, rect.x + rect.w, rect.y + rect.h), solid_color_brush_);
}

void Direct2DRenderer::SetDrawColor(Color color) {
  m_Color = D2D1::ColorF(color.r / 255.0f , color.g / 255.0f , color.b / 255.0f , color.a / 255.0f);
  solid_color_brush_->SetColor(m_Color);
}

bool Direct2DRenderer::InternalLoadFont(Font* pFont) {
  IDWriteTextFormat* pTextFormat = NULL;

  HRESULT hr = m_pDWriteFactory->CreateTextFormat(
      pFont->facename.c_str(),
      NULL,
      pFont->bold ? DWRITE_FONT_WEIGHT_BOLD : DWRITE_FONT_WEIGHT_NORMAL,
      DWRITE_FONT_STYLE_NORMAL,
      DWRITE_FONT_STRETCH_NORMAL,
      pFont->size,
      L"",
      &pTextFormat
     );

  if (SUCCEEDED(hr))
  {
    FontData* pFontData = new FontData();

    pFontData->pTextFormat = pTextFormat;

    pFont->data = pFontData;
    return true;
  }

  return false;
}

void Direct2DRenderer::LoadFont(Font* pFont) {
  if (InternalLoadFont(pFont))
    m_FontList.push_back(pFont);
}

void Direct2DRenderer::InternalFreeFont(Font* pFont, bool bRemove) {
  if (bRemove)
    m_FontList.remove(pFont);

  if (!pFont->data) return;

  FontData* pFontData = (FontData*) pFont->data;

  pFontData->pTextFormat->Release();

  delete pFontData;
  pFont->data = NULL;
}

void Direct2DRenderer::FreeFont(Font* pFont) {
  InternalFreeFont(pFont);
}

void Direct2DRenderer::RenderText(Font* pFont, Point pos, const string16& text) {
  // If the font doesn't exist, or the font size should be changed
  if (!pFont->data)
  {
    InternalFreeFont(pFont, false);
    InternalLoadFont(pFont);
  }

  FontData* pFontData = (FontData*) pFont->data;

  TranslateByRenderOffset(&pos.x, &pos.y);

  if (solid_color_brush_)
  {
    m_pRT->DrawTextW(text.c_str(), text.length(), pFontData->pTextFormat, D2D1::RectF(pos.x, pos.y, pos.x + 50000, pos.y + 50000), solid_color_brush_);
  }
}

Point Direct2DRenderer::MeasureText(Font* pFont, const string16& text) {
  // If the font doesn't exist, or the font size should be changed
  if (!pFont->data)
  {
    InternalFreeFont(pFont, false);
    InternalLoadFont(pFont);
  }

  FontData* pFontData = (FontData*) pFont->data;

  Point size;
  IDWriteTextLayout* pLayout;
  DWRITE_TEXT_METRICS metrics;

  m_pDWriteFactory->CreateTextLayout(text.c_str(), text.length(), pFontData->pTextFormat, 50000, 50000, &pLayout);

  pLayout->GetMetrics(&metrics);

  pLayout->Release();

  return Point(metrics.widthIncludingTrailingWhitespace, metrics.height);

}

void Direct2DRenderer::DeviceLost() {
  if (solid_color_brush_ != NULL)
  {
    solid_color_brush_->Release();
    solid_color_brush_ = NULL;
  }

  for (std::list<Texture*>::const_iterator tex_it = m_TextureList.begin(); tex_it != m_TextureList.end(); ++tex_it)
  {
    InternalFreeTexture(*tex_it, false);
  }
}

void Direct2DRenderer::DeviceAcquired(ID2D1RenderTarget* pRT) {
  m_pRT = pRT;

  HRESULT hr = m_pRT->CreateSolidColorBrush(m_Color, &solid_color_brush_);
  (void)hr;

  for (std::list<Texture*>::const_iterator tex_it = m_TextureList.begin(); tex_it != m_TextureList.end(); ++tex_it)
  {
    InternalLoadTexture(*tex_it);
  }
}

void Direct2DRenderer::StartClip() {
  Rect rect = ClipRegion();

  D2D1_RECT_F r = D2D1::RectF(rect.x, rect.y, (rect.x + rect.w), (rect.y + rect.h));

  m_pRT->PushAxisAlignedClip(r, D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);
}

void Direct2DRenderer::EndClip() {
  m_pRT->PopAxisAlignedClip();
}

void Direct2DRenderer::DrawTexturedRectAlpha(Texture* texture, Rect rect, float alpha, float u1, float v1, float u2, float v2) {
  TextureData* pTexData = (TextureData*) texture->data;

  // Missing image, not loaded properly?
  if (!pTexData || pTexData->pBitmap == NULL) 
    return DrawMissingImage(rect);

  TranslateByRenderOffset(&rect);

  m_pRT->DrawBitmap(pTexData->pBitmap,
      D2D1::RectF(rect.x, rect.y, rect.x + rect.w, rect.y + rect.h), 
      alpha, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR,
      D2D1::RectF(u1 * texture->width, v1 * texture->height, u2 * texture->width, v2 * texture->height)
     );
}

bool Direct2DRenderer::InternalLoadTexture(Texture* pTexture) {
  IWICBitmapDecoder *pDecoder = NULL;
  IWICBitmapFrameDecode *pSource = NULL;
  IWICFormatConverter *pConverter = NULL;
  ID2D1Bitmap     *pD2DBitmap = NULL;

  HRESULT hr = m_pWICFactory->CreateDecoderFromFilename(
      pTexture->name.c_str(),
      NULL,
      GENERIC_READ,
      WICDecodeMetadataCacheOnLoad,
      &pDecoder
     );

  if (SUCCEEDED(hr))
  {
    hr = pDecoder->GetFrame(0, &pSource);
  }

  if (SUCCEEDED(hr))
  {
    // Convert the image format to 32bppPBGRA
    // (DXGI_FORMAT_B8G8R8A8_UNORM + D2D1_ALPHA_MODE_PREMULTIPLIED).
    hr = m_pWICFactory->CreateFormatConverter(&pConverter);
  }

  if (SUCCEEDED(hr))
  {
    hr = pConverter->Initialize(
        pSource,
        GUID_WICPixelFormat32bppPBGRA,
        WICBitmapDitherTypeNone,
        NULL,
        0.f,
        WICBitmapPaletteTypeMedianCut
       );
  }

  if (SUCCEEDED(hr))
  {
    hr = m_pRT->CreateBitmapFromWicBitmap(
        pConverter,
        NULL,
        &pD2DBitmap
       );
  }

  if (SUCCEEDED(hr))
  {
    TextureData* texdata = new TextureData();

    texdata->pWICBitmap = pSource;
    texdata->pBitmap = pD2DBitmap;

    pTexture->data = texdata;

    D2D1_SIZE_F size = texdata->pBitmap->GetSize();

    pTexture->width = size.width;
    pTexture->height = size.height;
    pTexture->failed = false;
  }
  else
  {
    pTexture->failed = true;
  }

  if (pDecoder != NULL)
    pDecoder->Release();

  if (pConverter != NULL)
    pConverter->Release();

  return SUCCEEDED(hr);
}

void Direct2DRenderer::LoadTexture(Texture* pTexture) {
  if (InternalLoadTexture(pTexture))
    m_TextureList.push_back(pTexture);
}

void Direct2DRenderer::InternalFreeTexture(Texture* pTexture, bool bRemove) {
  if (bRemove)
    m_TextureList.remove(pTexture);

  if (pTexture->data != NULL)
  {
    TextureData* texdata = (TextureData*)pTexture->data;

    if (texdata->pWICBitmap != NULL)
      texdata->pWICBitmap->Release();

    if (texdata->pBitmap != NULL)
      texdata->pBitmap->Release();

    delete texdata;
  }

  pTexture->data = NULL;
}

void Direct2DRenderer::FreeTexture(Texture* pTexture)
{
  InternalFreeTexture(pTexture);
}

void Direct2DRenderer::Release() {
  std::list<Texture*>::iterator tex_it = m_TextureList.begin();

  while (tex_it != m_TextureList.end())
  {
    FreeTexture(*tex_it);
    tex_it = m_TextureList.begin();
  }

  std::list<Font*>::iterator it = m_FontList.begin();

  while (it != m_FontList.end())
  {
    FreeFont(*it);
    it = m_FontList.begin();
  }
}
