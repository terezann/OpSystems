#include <stdio.h>
#include <fcntl.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <string.h>
#include <sys/stat.h>
#include <stdbool.h>
#include <signal.h>
#include <sys/resource.h>
#include <errno.h>

pid_t pid_array[50];
pid_t rc_pid = 0;
char *c;
int g;

void handle_sigusr1(int sig) {
    for (int i = 0; i < strlen(c); i++) kill(pid_array[i], SIGUSR1);
}

void handle_sigusr2(int sig) {
    for (int i = 0; i < strlen(c); i++) kill(pid_array[i], SIGUSR2);
}

void handle_sigterm(int sig) {
    printf("Parent: HAVE TO KILL 'EM ALL\n");
    for (int i = 0; i < strlen(c); i++) {
        printf("Waiting for %ld children to exit\n", (strlen(c)-i));
        kill(pid_array[i], SIGTERM);
    }
    exit(EXIT_SUCCESS);
}


bool healthy_child(int pid, const char *msg) {
    if (pid < 0) {
        perror(msg);
        return false;
    }
    return true;
}

pid_t make_child(char cc, int no) {
        pid_t kiddo;
        kiddo = fork();

        if (!healthy_child(kiddo, "Child failed\n")) exit(EXIT_FAILURE);

        if (kiddo == 0) { //child process

            printf("[PARENT/PID=%d] Created child %d (PID=%d) and initial state '%c'\n", getppid(), no, getpid(), cc); /* child, load "./child" executable */ 
            char *const argv[] = {"./child", &cc, NULL};
            int status = execv("./child", argv);
        }

        return kiddo;
}

pid_t remake_child(char cc, int no) {
        pid_t kiddo;
        kiddo = fork();

        if (!healthy_child(kiddo, "Child failed\n")) exit(EXIT_FAILURE);

        if (kiddo == 0) { //child process
            if (cc == 't') g = 1;
            if (cc == 'f') g = 0;
            printf("[PARENT/PID=%d] Created new child for gate %d (PID=%d) and initial state '%c'\n", getppid(), g, getpid(), cc); 
            /* child, load "./child" executable */ 
            char *const argv[] = {"./child", &cc, NULL};
            int status = execv("./child", argv);
        }

        return kiddo;
}



void describe_wait_status(pid_t pid, int status) {
    if (pid < 1) {
        perror("wait() call failed");
    }

    if (pid == 0) {
        printf("Nothing happened");
    }

    if (WIFSTOPPED(status)) {
        printf("Child with PID %d stopped\n", pid);
    } 
    else if (WIFCONTINUED(status)) {
        printf("Child with PID %d continued\n", pid);
    } 
    else if (WIFEXITED(status)) {
        printf("Child with PID %d exited with status code %d\n", pid,
                WEXITSTATUS(status));
    } 
    else if (WIFSIGNALED(status)) {
        printf("Child with PID %d terminated by signal %d with status code %d\n",
                pid, WSTOPSIG(status), WEXITSTATUS(status));
    }
}




int main (int argc, char *argv[]) {
    
    if (argc != 2) {
        printf("Usage: ./a.out children\n");
        exit(EXIT_FAILURE);
    }

    c = argv[1];
    
    for (int i = 0; i < strlen(c); i++) {
        if (argv[1][i] != 't' && argv[1][i] != 'f') {
            printf("children using only t or f characters\n");
            exit(EXIT_FAILURE);
        }
    }

    for (int i = 0; i < strlen(c); i++){
        pid_array[i] = make_child(c[i],i);
    }

    struct sigaction sat = {0};
    struct sigaction sa1 = {0};
    struct sigaction sa2 = {0};

    sat.sa_handler = &handle_sigterm;
    sa1.sa_handler = &handle_sigusr1;
    sa2.sa_handler = &handle_sigusr2;
    
    sat.sa_flags = SA_RESTART;
    sa1.sa_flags = SA_RESTART;
    sa2.sa_flags = SA_RESTART;

    sigaction(SIGTERM, &sat, NULL);
    sigaction(SIGUSR1, &sa1, NULL);
    sigaction(SIGUSR2, &sa2, NULL);

    // while (1) {
    //     printf("a\n");
    //     int status;
    //     pid_t pid = wait(&status);
    //     for (int i = 0; i < strlen(c); i++) {
    //         describe_wait_status(pid_array[i], status);
    //         sleep(2);
    //     }
    // }

    while (rc_pid != -1) {
        int chld_state;
        rc_pid = waitpid(-1, &chld_state, WNOHANG | WUNTRACED);
        if (rc_pid == 0) continue;
        if (rc_pid == -1) {
            perror("waitpid");
            exit(EXIT_FAILURE);
        }
        if (WIFEXITED(chld_state) || WIFSIGNALED(chld_state)) {
            for (int i = 0; i < strlen(c); i++) {
                if (rc_pid == pid_array[i]) {
                    describe_wait_status(rc_pid, chld_state);
                    pid_array[i] = remake_child(c[i], i);
                }
            }
        }

        // rc_pid = waitpid(-1, &chld_state, WUNTRACED);
        // describe_wait_status(rc_pid, chld_state);
        // if (rc_pid == 0) continue;
        if (WIFSTOPPED(chld_state)) { 
            describe_wait_status(rc_pid, chld_state);
            kill(rc_pid, SIGCONT);
        }
    }

    return 0;
    
}
