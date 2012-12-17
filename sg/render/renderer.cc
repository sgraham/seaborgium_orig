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

Renderer::Renderer() : render_offset_(0, 0) {
}

Renderer::~Renderer() {
}

void Renderer::TranslateByRenderOffset(int* x, int* y) {
  *x += render_offset_.x;
  *y += render_offset_.y;
}

void Renderer::TranslateByRenderOffset(Rect* rect) {
  TranslateByRenderOffset(&rect->x, &rect->y);
}

void Renderer::SetClipRegion(Rect rect) {
  clip_region_ = rect;
}

void Renderer::AddClipRegion(Rect rect) {
  rect.x = render_offset_.x;
  rect.y = render_offset_.y;

  Rect out = rect;
  if (rect.x < clip_region_.x) {
    out.w -= (clip_region_.x - out.x);
    out.x = clip_region_.x;
  }

  if (rect.y < clip_region_.y) {
    out.h -= (clip_region_.y - out.y);
    out.y = clip_region_.y;
  }

  if (rect.x + rect.w > clip_region_.x + clip_region_.w)
    out.w = (clip_region_.x + clip_region_.w) - out.x;

  if (rect.y + rect.h > clip_region_.y + clip_region_.h)
    out.h = (clip_region_.y + clip_region_.h) - out.y;

  clip_region_ = out;
}

const Rect& Renderer::ClipRegion() const {
  return clip_region_;
}

bool Renderer::ClipRegionVisible() {
  if (clip_region_.w <= 0 || clip_region_.h <= 0)
    return false;
  return true;
}

void Renderer::DrawMissingImage(Rect pTargetRect) {
  SetDrawColor(Color(255, 0, 0));
  DrawFilledRect(pTargetRect);
}

void Renderer::DrawTexturedRect(
    Texture* texture,
    Rect target_rect,
    float u1, float v1, float u2, float v2) {
  DrawTexturedRectAlpha(texture, target_rect, 1.0, u1, v1, u2, v2);
}

void Renderer::DrawOutlineRect(Rect rect) {
  DrawFilledRect(Rect(rect.x, rect.y, rect.w, 1));
  DrawFilledRect(Rect(rect.x, rect.y + rect.h - 1, rect.w, 1));
  DrawFilledRect(Rect(rect.x, rect.y, 1, rect.h));
  DrawFilledRect(Rect(rect.x + rect.w - 1, rect.y, 1, rect.h));
}

RenderToTextureRenderer::RenderToTextureRenderer() {
}

RenderToTextureRenderer::~RenderToTextureRenderer() {
}
