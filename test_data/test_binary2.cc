// Copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <stdio.h>
#include <windows.h>

#include <vector>

std::vector<int> GetStuff() {
  std::vector<int> to_return;
  to_return.push_back(44);
  to_return.push_back(88);
  to_return.push_back(99);
  return to_return;
}

int SubFunction(int input) {
  return input * 10;
}

int Function() {
  int x = SubFunction(44);
  return x;
}

int main(int argc, char** argv) {
  printf("This is the test binary!\n");
  printf("argc: %d\n", argc);
  double f = 42.432;
  for (int i = 0; i < argc; ++i) {
    printf("argv[%d]: %s\n", i, argv[i]);
  }
  printf("f: %f\n", f);
  int result = Function();
  printf("result: %d\n", result);
  printf("Function(): %d\n", Function());
  std::vector<int> stuff = GetStuff();
  for (size_t i = 0; i < stuff.size(); ++i) {
    printf("%d\n", stuff.at(i));
  }
  Sleep(10000);
  printf("after sleep\n");
  return 0;
}
