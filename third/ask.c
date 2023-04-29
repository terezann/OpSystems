#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/select.h>
#include <string.h>
#include <signal.h>
#include <sys/wait.h>

#define max(x, y) (((x) > (y)) ? (x) : (y))
#define BUFSIZE 256

int main(int argc, char *argv[]) {
    int pid_array[100];
    int pipe1fd[2];
    int pipe2fd[2];
    char buffer1[BUFSIZE];
    char buffer2[BUFSIZE];
    char buffer_c[BUFSIZE];
    fd_set readfds;
    int maxfd;
    int n = atoi(argv[1]);

    for (int i = 0; i < n; i++) {
        int pipe1fd[i];
    }

    if(pipe(pipe1fd) < 0 || pipe(pipe2fd) < 0) {
        perror("pipe\n");
        exit(EXIT_FAILURE);
    }


    for (int i = 0; i < n; i++) {
        pid_t pid = fork();
        pid_array[i] = pid;
        if (pid < 0) {
            perror("fork\n");
            exit(EXIT_FAILURE); 
        }
        
        if (pid == 0) { 
            while (1) {
                int gIndex = i;
                int n1 = read(pipe1fd[0], buffer_c, sizeof(int));

                if (n1 <= 0) {
                    perror("child read to pipe\n");
                    exit(EXIT_FAILURE);
                }

                int num = atoi(buffer_c);
                printf("[Child %d] [%d] Child received %d!\n", gIndex, getpid(), num);
                num++;

                sleep(5);

                sprintf(buffer_c, "%d", num);
                // strcpy(buffer2, itoa(num));

                int n2 = write(pipe2fd[1], buffer_c, sizeof(int));
                if (n2 <= 0) {
                    perror("child write to pipe\n");
                    exit(EXIT_FAILURE);
                }
                printf("[Child %d] [%d] Child finished hard work, writing back %d\n", gIndex, getpid(), num);
            }
        }

    }
    
    //else --> parent process
    FD_ZERO(&readfds);
    FD_SET(STDIN_FILENO, &readfds);
    FD_SET(pipe2fd[0], &readfds);
    maxfd = max(pipe2fd[0], STDIN_FILENO);
    while(1) {

        if (select(maxfd + 1, &readfds, NULL, NULL, NULL) < 0) {
            perror("select\n");
            continue;
        }
        
        if (FD_ISSET(STDIN_FILENO, &readfds)) {
            //read data from stdin
            int n = read(STDIN_FILENO, buffer1, BUFSIZE); //thelei akrivws oso einai to noumero alla tha to ftiaksw se 2h fash.

            buffer1[n] = '\0';

            if (n <= 0) {
                perror("read from stdin\n");
                exit(EXIT_FAILURE);
            }

            if (n > 0 && buffer1[n-1] == '\n') {
                buffer1[n-1] = '\0';
            }

            if (n >= 4 && strncmp(buffer1, "exit", 4) == 0) {
                // user typed 'exit', kill child and exit properly
                for (int i = 0; i < atoi(argv[1]); i++) {
                    kill(pid_array[i], SIGTERM);
                    printf("Waiting for child %d\n", i);
                }
                exit(EXIT_SUCCESS);
            }

            printf("Received input from stdin: %s\n", buffer1);

            //write user's input to the suitable pipe.
            //estw seiriaka, tha ginei meta to random.

            if (write(pipe1fd[1], buffer1, BUFSIZE) < 0) {
                perror("write to pipe1\n");
                exit(EXIT_FAILURE);
            }
            else {
                printf("Assigned %s to child\n", buffer1);
            }
        }

        for (int i = 0; i < atoi(argv[1]); i++) {
            if (FD_ISSET(pipe2fd[0], &readfds)) {
                int n = read(pipe2fd[0], buffer2, sizeof(int));
                if (n <= 0) {
                    perror("read from pipe\n");
                    exit(EXIT_FAILURE);
                }
                printf("Received message from child nolala: %s\n", buffer2);
            }
        }

        int n = write(pipe1fd[1], buffer1, BUFSIZE);
        if (n < 0) {
            perror("write to pipe\n");
            exit(EXIT_FAILURE);
        }
        printf("Writing to the child?\n");
        sleep(2);

    }
    
}
