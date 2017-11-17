#include "signal_handlers.h"
#include <signal.h>

void catch_sigint(int signalNo)
{
  // TODO: File this!
  if(signalNo == SIGINT) {
    printf("SIGINT\n");
    return;
  }
}

void catch_sigtstp(int signalNo)
{
  // TODO: File this!
  if(signalNo == SIGTSTP) {
    printf("SIGTSTP\n");
    return;
  }
}
