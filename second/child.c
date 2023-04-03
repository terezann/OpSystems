#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>

int GATE;
int t = 0;

void handle_sigterm(int sig) {
    print
}

void handle_sigusr1(int sig) {
    printf("State flipped\n");
    if (GATE == 0) {
        GATE = 1;
    }
    else GATE = 0;
}

void handle_sigusr2(int sig) {
    printf("State remains the same.\n");
}

void handle_sigalrm(int sig) {
    if (GATE == 0) {
        printf("[GATE=%d/PID=%d/TIME=%ds] The gates are closed!\n", GATE, getpid(), t);
    }
    else {
        printf("[GATE=%d/PID=%d/TIME=%ds] The gates are open!\n", GATE, getpid(), t);
    }
    alarm(5);
}

int main(int argc, char **argv) {

    printf("argv[1]=%d\n", *argv[1]);

    struct sigaction sa1 = {0};
    struct sigaction sa2 = {0};
    struct sigaction sal = {0};

    sa1.sa_handler = &handle_sigusr1;
    sa2.sa_handler = &handle_sigusr2;
    sal.sa_handler = &handle_sigalrm;
    
    sa1.sa_flags = SA_RESTART;
    sa2.sa_flags = SA_RESTART;
    sal.sa_flags = SA_RESTART;

    sigaction(SIGUSR1, &sa1, NULL);
    sigaction(SIGUSR2, &sa2, NULL);
    sigaction(SIGALRM, &sal, NULL);

    alarm(5);
    while (1) {
        printf("inside the while loop\n");
        pause();
    }
    return 0;
}
