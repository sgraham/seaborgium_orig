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

#ifndef SG_RENDER_RENDERER_H_
#define SG_RENDER_RENDERER_H_

#include "base/string16.h"
#include "sg/basic_geometric_types.h"

class Texture;

class Font;
class WindowProvider;

class Renderer
{
 public:

  Renderer();
  virtual ~Renderer();

  virtual void Init(){};

  virtual void Begin(){};
  virtual void End(){};

  virtual void SetDrawColor( Color color ){};

  virtual void DrawFilledRect( Rect rect ){};;

  virtual void StartClip(){};
  virtual void EndClip(){};

  virtual void LoadTexture( Texture* pTexture ){};
  virtual void FreeTexture( Texture* pTexture ){};
  virtual void DrawTexturedRect( Texture* pTexture, Rect pTargetRect, float u1=0.0f, float v1=0.0f, float u2=1.0f, float v2=1.0f ){};
  virtual void DrawTexturedRectAlpha( Texture* pTexture, Rect pTargetRect, float alpha, float u1=0.0f, float v1=0.0f, float u2=1.0f, float v2=1.0f ){};
  virtual void DrawMissingImage( Rect pTargetRect );
  virtual Color PixelColour( Texture* pTexture, unsigned int x, unsigned int y, const Color& col_default = Color( 255, 255, 255, 255 ) ){ return col_default; }


  virtual void LoadFont( Font* pFont ){};
  virtual void FreeFont( Font* pFont ){};
  virtual void RenderText( Font* pFont, Point pos, const string16& text );
  virtual Point MeasureText( Font* pFont, const string16& text );

  //
  // No need to implement these functions in your derived class, but if 
  // you can do them faster than the default implementation it's a good idea to.
  //
  virtual void DrawLinedRect( Rect rect );
  virtual void DrawPixel( int x, int y );
  virtual void DrawShavedCornerRect( Rect rect, bool bSlight = false );

 public:

  //
  // Translate a panel's local drawing coordinate
  //  into view space, taking Offset's into account.
  //
  void Translate( int& x, int& y );
  void Translate( Rect& rect );

  //
  // Set the rendering offset. You shouldn't have to 
  // touch these, ever.
  //
  void SetRenderOffset( const Point& offset ){ m_RenderOffset = offset; }
  void AddRenderOffset( const Rect& offset ){ m_RenderOffset.x += offset.x; m_RenderOffset.y += offset.y; }
  const Point& GetRenderOffset() const { return m_RenderOffset; }

 private:

  Point m_RenderOffset;

 public:

  void SetClipRegion( Rect rect );
  void AddClipRegion( Rect rect );
  bool ClipRegionVisible();
  const Rect& ClipRegion() const;

 private:

  Rect m_rectClipRegion;

 public:

  void SetScale( float fScale ){ m_fScale = fScale; }
  float Scale() const { return m_fScale; }

  float m_fScale;
};

#endif  // SG_RENDER_RENDERER_H_
