#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdint-gcc.h>

volatile sig_atomic_t exit_trigger = 0;

void Handler(int sig, siginfo_t* info, void* ucontext) {
  int32_t num = info->si_int - 1;
  if (num == -1) {
    exit_trigger = 1;
    return;
  }
  union sigval value;
  value.sival_int = num;
  sigqueue(info->si_pid, sig, value);
}

typedef struct sigaction sig_act;

int main(int argc, char* argv[]) {
  sigset_t mask;
  sigfillset(&mask);
  sigdelset(&mask, SIGRTMIN);
  sigprocmask(SIG_SETMASK, &mask, NULL);
  sig_act rt_min;
  memset(&rt_min, 0, sizeof(rt_min));
  rt_min.sa_sigaction = Handler;
  rt_min.sa_flags = SA_SIGINFO;
  sigaction(SIGRTMIN, &rt_min, NULL);
  while (!exit_trigger) {
    pause();
  }
  return 0;
}
