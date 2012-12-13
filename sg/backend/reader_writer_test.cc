// Copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <stdio.h>
#include <windows.h>

void DoRead() {
  char buf[4 << 10];
  fread(buf, 1, sizeof(buf), stdin);
}

void DoWrite() {
  fwrite("some stuff\n", 1, 11, stdout);
  fflush(stdout);
}

// A trivial app to test subprocess handle hookups.
int main(int argc, char** argv) {
  if (argc != 2)
    abort();
  for (int i = 0; i < strlen(argv[1]); ++i) {
    if (argv[1][i] == 'r')
      DoRead();
    else if (argv[1][i] == 'w')
      DoWrite();
    else if (argv[1][i] == 's')
      Sleep(1000);
    else
      abort();
  }
  return 0;
}
