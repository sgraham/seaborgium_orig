#ifndef MAIN_LOOP_H_
#define MAIN_LOOP_H_

#include "base/basictypes.h"
#include "base/memory/scoped_ptr.h"
#include "base/message_loop.h"

class MainLoop {
 public:
  MainLoop();
  ~MainLoop();

  void Init();
  void MainMessageLoopStart();
  void CreateThreads();
  void MainMessageLoopRun();
  void ShutdownThreadsAndCleanUp();
  int GetResultCode();

 private:
  scoped_ptr<MessageLoop> main_message_loop_;

  int result_code_;

  DISALLOW_COPY_AND_ASSIGN(MainLoop);
};

#endif  // MAIN_LOOP_H_
