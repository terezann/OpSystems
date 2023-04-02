#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int GATE;

void handler_sigusr1(int sig) {
    printf("State flipped\n");
    if (GATE == 0) {
        GATE = 1;
    }
    else GATE = 0;
}

int main(int argc, char **argv) {

    while (1) {
        printf("Child process with id: %d is still alive.\n", getpid());
        sleep(3);
    }
    

  return 0;
}
