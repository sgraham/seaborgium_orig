#include "base/at_exit.h"
#include "base/command_line.h"
#include "main_loop.h"

int RunMain(HINSTANCE instance) {
  scoped_ptr<MainLoop> main_loop(new MainLoop);
  main_loop->Init();
  main_loop->MainMessageLoopStart();
  main_loop->CreateThreads();
  main_loop->MainMessageLoopRun();
  main_loop->ShutdownThreadsAndCleanUp();
  return main_loop->GetResultCode();
}

int APIENTRY wWinMain(HINSTANCE instance, HINSTANCE prev, wchar_t*, int) {
  CommandLine::Init(0, NULL);
  base::AtExitManager exit_manager;
  return RunMain(instance);
}
