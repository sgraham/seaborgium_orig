// // Copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Hopefully this will be first binary we can load successfully.

#include <stdio.h>
#include <windows.h>

int main(int argc, char** argv) {
  printf("This is the test binary!\n");
  printf("argc: %d\n", argc);
  double f = 42.432;
  for (int i = 0; i < argc; ++i) {
    printf("argv[%d]: %s\n", i, argv[i]);
  }
  printf("f: %f\n", f);
  Sleep(10000);
  printf("after sleep\n");
  return 0;
}
