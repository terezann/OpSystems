#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>

int x = 0;

void handle_sigusr1(int sig) {
    if (x == 0) {
        printf("\n(HINT) Remember that multiplication is repetitive addition!\n");
    }
}

int main (int argc, char *argv[]) {
    int pid = fork();
    
    if (pid == -1) return 1;

    if (pid == 0) {
        // Child Process
        sleep(5);
        kill(getppid(), SIGUSR1);
    }

    else {
        struct sigaction sa = { 0 }; //initialization of all the members of the struct
        sa.sa_flags = SA_RESTART; //because we're using scanf and we want to restart scanf once it has finished it's execution.
                                  //if we're not using scanf, we don't have to set this flag.
        sa.sa_handler = &handle_sigusr1;
        sigaction(SIGUSR1, &sa, NULL);

        // Parent process
        printf("What is the result of 3 x 5: ");
        scanf("%d", &x);
        if (x == 15) {
            printf("Right!");
        }
        else {
            printf("Wrong!");
        }
        wait(NULL);
    }

    return 0;
}
