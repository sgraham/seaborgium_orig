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

#include "sg/render/renderer.h"
#include "sg/render/font.h"

#include <math.h>

Renderer::Renderer()
{
    m_RenderOffset = Point( 0, 0 );
    m_fScale = 1.0f;
}

Renderer::~Renderer()
{
}

void Renderer::Translate( int& x, int& y )
{
    x += m_RenderOffset.x;
    y += m_RenderOffset.y;

    x = ceil( ((float) x ) * m_fScale );
    y = ceil( ((float) y ) * m_fScale );
}

void Renderer::Translate( Rect& rect )
{
    Translate( rect.x, rect.y );

    rect.w = ceil(((float) rect.w ) * m_fScale);
    rect.h = ceil(((float) rect.h ) * m_fScale);
}

void Renderer::SetClipRegion( Rect rect )
{ 
    m_rectClipRegion = rect; 
}

void Renderer::AddClipRegion( Rect rect ) 
{ 
    rect.x = m_RenderOffset.x;
    rect.y = m_RenderOffset.y;

    Rect out = rect;
    if ( rect.x < m_rectClipRegion.x )
    {
        out.w -= ( m_rectClipRegion.x - out.x );
        out.x = m_rectClipRegion.x;
    }

    if ( rect.y < m_rectClipRegion.y )
    {
        out.h -= ( m_rectClipRegion.y - out.y );
        out.y = m_rectClipRegion.y;
    }

    if ( rect.x + rect.w > m_rectClipRegion.x + m_rectClipRegion.w )
    {
        out.w = (m_rectClipRegion.x + m_rectClipRegion.w) - out.x;
    }

    if ( rect.y + rect.h > m_rectClipRegion.y + m_rectClipRegion.h )
    {
        out.h = (m_rectClipRegion.y + m_rectClipRegion.h) - out.y;
    }

    m_rectClipRegion = out;
}

const Rect& Renderer::ClipRegion() const
{ 
    return m_rectClipRegion; 
}

bool Renderer::ClipRegionVisible()
{
    if ( m_rectClipRegion.w <= 0 || m_rectClipRegion.h <= 0 )
        return false;

    return true;
}

void Renderer::DrawMissingImage( Rect pTargetRect ) {
  SetDrawColor( Color(255, 0, 0) );
  DrawFilledRect( pTargetRect );
}
