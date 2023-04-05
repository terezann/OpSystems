#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>

int GATE;
char *G = "t";
char *num;
time_t start, now;

void handle_sigterm(int sig) {
    now = 0;
    time(&now);
    printf("child exited successfully yadayada\n");
    exit(EXIT_SUCCESS); //or exit_failure?
}

void handle_sigusr1(int sig) {
    now = 0;
    time(&now);
    if (GATE == 1) {
        GATE = 0;
        printf("[GATE = %s/PID = %d/TIME = %lds] The gates are closed!\n", num, getpid(), (long int)(now - start));
    }
    else {
        GATE = 0;
        printf("[GATE = %s/PID = %d/TIME = %lds] The gates are open!\n", num, getpid(), (long int)(now - start));
    }
}

void handle_sigusr2(int sig) {
    now = 0;
    time(&now);
    if (GATE == 0) {
        printf("[GATE = %s/PID = %d/TIME = %lds] The gates are closed!\n", num, getpid(), (long int)(now - start));
    }
    else {
        printf("[GATE = %s/PID = %d/TIME = %lds] The gates are open!\n", num, getpid(), (long int)(now - start));
    }
}

void handle_sigalrm(int sig) {
    now = 0;
    time(&now);
    if (GATE == 0) {
        printf("[GATE=%s/PID=%d/TIME=%lds] The gates are closed!\n", num, getpid(), (long int)(now - start));
    }
    else {
        printf("[GATE=%s/PID=%d/TIME=%lds] The gates are open!\n", num, getpid(), (long int)(now - start));
    }

    alarm(15);

    while (sig != -1) {
        pause();
    }
}

int main(int argc, char **argv) {

    struct sigaction sat = {0};
    struct sigaction sa1 = {0};
    struct sigaction sa2 = {0};
    struct sigaction sal = {0};

    sat.sa_handler = &handle_sigterm;
    sa1.sa_handler = &handle_sigusr1;
    sa2.sa_handler = &handle_sigusr2;
    sal.sa_handler = &handle_sigalrm;

    sigemptyset(&sal.sa_mask);
    sal.sa_flags = SA_NODEFER;
    
    // sa1.sa_flags = SA_RESTART;
    // sa2.sa_flags = SA_RESTART;

    sigaction(SIGTERM, &sat, NULL);
    sigaction(SIGUSR1, &sa1, NULL);
    sigaction(SIGUSR2, &sa2, NULL);
    sigaction(SIGALRM, &sal, NULL);

    if (*argv[1] == *G){ 
        GATE = 1;
    }
    else { 
        GATE = 0;
    }

    num = argv[2];

    if (GATE == 0) { 
        printf("[GATE = %s/PID = %d/TIME = %lds] The gates are closed!\n", num, getpid(), (long int)(now - start));
    } else { 
        printf("[GATE = %s/PID = %d/TIME = %lds] The gates are open!\n", num, getpid(), (long int)(now - start));
    }

    time(&start);

    alarm(15);
    pause();

    // while (1) {
    //     printf("inside the while loop\n");
    //     pause();
    // }

    return 0;
}
