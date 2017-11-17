#include "signal_handlers.h"
#include <signal.h>
#include <stdio.h>

void catch_sigint(int signalNo)
{
  // TODO: File this!
  if(signalNo == SIGINT) {
    signal(SIGINT, SIG_IGN);
    return;
  }
  fprintf(stderr, "Signal Error!\n");
}

void catch_sigtstp(int signalNo)
{
  // TODO: File this!
  if(signalNo == SIGTSTP) {
    signal(SIGTSTP, SIG_IGN);
    return;
  }
  fprintf(stderr, "Signal Error!\n");
}
