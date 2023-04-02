#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <unistd.h>

void handle_sigtstp(int sig) {
    printf("Stop not allowed\n");
}

void handle_sigcont(int sig) {
    printf("Input number: ");
    fflush(stdout);
}

int main(int argc, char* argv[]) {
    struct sigaction sa;
    sa.sa_handler = &handle_sigtstp;
    sa.sa_flags = SA_RESTART;
    sigaction(SIGTSTP, &sa, NULL);

    int x;
    printf("Input Number: ");
    scanf("%d", &x);
    printf("Result %d * 5 = %d\n", x, x*5);
}

