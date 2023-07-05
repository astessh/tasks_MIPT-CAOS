#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>

volatile sig_atomic_t num = 0;
volatile sig_atomic_t exit_trigger = 0;

void SIGUSR1Handler() {
  num++;
  printf("%d ", num);
  fflush(stdout);
}

void SIGUSR2Handler() {
  num *= -1;
  printf("%d ", num);
  fflush(stdout);
}

void TERMHandler() {
  exit_trigger = 1;
}

int main() {
  printf("%d\n", getpid());
  scanf("%d", &num);
  struct sigaction usr1;
  memset(&usr1, 0, sizeof(usr1));
  usr1.sa_handler = SIGUSR1Handler;
  usr1.sa_flags = SA_RESTART;
  sigaction(SIGUSR1, &usr1, NULL);
  struct sigaction usr2;
  memset(&usr2, 0, sizeof(usr2));
  usr2.sa_handler = SIGUSR2Handler;
  usr2.sa_flags = SA_RESTART;
  sigaction(SIGUSR2, &usr2, NULL);
  struct sigaction term;
  memset(&term, 0, sizeof(term));
  term.sa_handler = TERMHandler;
  term.sa_flags = SA_RESTART;
  sigaction(SIGTERM, &term, NULL);
  sigaction(SIGINT, &term, NULL);
  fflush(stdout);
  while (!exit_trigger) {
    pause();
  }
  return 0;
}
