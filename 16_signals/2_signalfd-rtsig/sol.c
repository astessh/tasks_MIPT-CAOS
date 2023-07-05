#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

volatile sig_atomic_t exit_trigger = 0;
char** files;

void SIGRTMINHandler() {
  exit_trigger = 1;
}

void SIGRTHandler(int id) {
  int file_ind = id - SIGRTMIN;
  char str[4096];
  FILE* in = fopen(files[file_ind], "r");
  fscanf(in, "%s", str);
  fclose(in);
  printf("%s\n", str);
  fflush(stdout);
}

typedef struct sigaction sig_act;

int main(int argc, char* argv[]) {
  int i = 1;
  files = argv;
  sigset_t mask;
  sigfillset(&mask);
  sig_act* sigs = (sig_act*) calloc((SIGRTMAX - SIGRTMIN), sizeof(sig_act));
  for (int sig_rt = SIGRTMIN; sig_rt < SIGRTMAX; sig_rt++) {
    sigdelset(&mask, sig_rt);
    i = sig_rt - SIGRTMIN;
    if (i != 0) {
      memset(&sigs[i], 0, sizeof(sigs[i]));
      sigs[i].sa_handler = SIGRTHandler;
      sigs[i].sa_flags = SA_RESTART;
      sigaction(sig_rt, &sigs[i], NULL);
    } else {
      memset(&sigs[i], 0, sizeof(sigs[i]));
      sigs[i].sa_handler = SIGRTMINHandler;
      sigs[i].sa_flags = SA_RESTART;
      sigaction(sig_rt, &sigs[i], NULL);
    }
  }
  sigprocmask(SIG_SETMASK, &mask, NULL);
  fflush(stdout);
  while (!exit_trigger) {
    pause();
  }
  return 0;
}
