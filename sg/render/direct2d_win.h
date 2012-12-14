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

#include "base/string16.h"
#include "sg/basic_geometric_types.h"
#include "sg/render/renderer.h"

class Font;
class Texture;

class Direct2DRenderer : public Renderer
{
 public:

  Direct2DRenderer();
  Direct2DRenderer( ID2D1RenderTarget* pDevice, IDWriteFactory* pDWriteFactory, IWICImagingFactory* pWICFactory );
  ~Direct2DRenderer();

  virtual void Begin();
  virtual void End();
  virtual void Release();

  virtual void SetDrawColor(Color color);

  virtual void DrawFilledRect( Rect rect );

  virtual void LoadFont( Font* pFont );
  virtual void FreeFont( Font* pFont );
  virtual void RenderText( Font* pFont, Point pos, const string16& text );
  virtual Point MeasureText( Font* pFont, const string16& text );

  virtual void DeviceLost();
  virtual void DeviceAcquired( ID2D1RenderTarget* pRT );

  void StartClip();
  void EndClip();

  void DrawTexturedRect( Texture* pTexture, Rect pTargetRect, float u1=0.0f, float v1=0.0f, float u2=1.0f, float v2=1.0f );
  void DrawTexturedRectAlpha( Texture* pTexture, Rect pTargetRect, float alpha, float u1=0.0f, float v1=0.0f, float u2=1.0f, float v2=1.0f );
  void LoadTexture( Texture* pTexture );
  void FreeTexture( Texture* pTexture );
  Color PixelColour( Texture* pTexture, unsigned int x, unsigned int y, const Color& col_default );

  void DrawLinedRect( Rect rect );
  void DrawShavedCornerRect( Rect rect, bool bSlight = false );

 private:
  bool InternalCreateDeviceResources();
  void InternalReleaseDeviceResources();

  ID2D1Factory* d2d_factory_;
  HWND hwnd_;

 private:
  bool InternalLoadTexture( Texture* pTexture );
  bool InternalLoadFont( Font* pFont );

  void InternalFreeFont( Font* pFont, bool bRemove = true );
  void InternalFreeTexture( Texture* pTexture, bool bRemove = true );

 private:

  IDWriteFactory*   m_pDWriteFactory;
  IWICImagingFactory* m_pWICFactory;
  ID2D1RenderTarget*  m_pRT;

  ID2D1SolidColorBrush* m_pSolidColorBrush;

  D2D1::ColorF    m_Color;

  std::list<Texture*> m_TextureList;
  std::list<Font*> m_FontList;

};

#endif  // SG_RENDER_DIRECT2D_WIN_H_
