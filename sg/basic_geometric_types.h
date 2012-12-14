// Copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef SG_BASIC_GEOMETRIC_TYPES_H_
#define SG_BASIC_GEOMETRIC_TYPES_H_

#include "Gwen/Structures.h"

class Rect {
 public:
  Rect() : x(0), y(0), w(-1), h(-1) {}
  Rect(int x, int y, int w, int h) : x(x), y(y), w(w), h(h) {}
  Rect Expand(const Rect& by) const {
    return Rect(x - by.x, y - by.y, w + by.x + by.w, h + by.y + by.w);
  }
  Rect Contract(const Rect& by) const {
    return Rect(x + by.x, y + by.y, w - by.x - by.w, h - by.y - by.h);
  }
  int x, y, w, h;
};

class Point {
 public:
  Point() : x(0), y(0) {}
  Point(int x, int y) : x(x), y(y) {}
  int x, y;
};

class Color {
 public:
  Color(unsigned char r, unsigned char g, unsigned char b)
      : r(r), g(g), b(b), a(255) {
  }

  Color(unsigned char r, unsigned char g, unsigned char b, unsigned char a)
      : r(r), g(g), b(b), a(a) {
  }

  // TODO(scottmg): Temp during conversion.
  operator Gwen::Color() const {
    return Gwen::Color(r, g, b, a);
  }

  unsigned char r, g, b, a;
};

#endif  // SG_BASIC_GEOMETRIC_TYPES_H_
