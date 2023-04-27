#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/select.h>

#define max(x, y) (((x) > (y)) ? (x) : (y))
#define BUFSIZE 256

int main(int argc, char *argv[]) {
    int pipe1fd[2];
    int pipe2fd[2];
    char buffer[BUFSIZE];
    // fd_set writefds;
    fd_set readfds;
    int maxfd;

    if(pipe(pipe1fd) < 0 || pipe(pipe2fd) < 0) {
        perror("pipe error");
        exit(EXIT_FAILURE);
    }

    pid_t pid = fork(); //ftiakse ena paidi

    if (pid < 0) { //elegxos paidiou
        perror("fork error");
        exit(EXIT_FAILURE);
    }

    if (pid == 0){ //child process
        // int val;
        int gIndex = 1; //number of child
        while(1) {

            int n1 = read(pipe1fd[0], buffer, sizeof(int));
            if (n1 <= 0) {
                perror("child read to pipe error\n");
                exit(EXIT_FAILURE);
            }
            int num = atoi(buffer);
            printf("[Child %d] [%d] Child received %d!\n", gIndex, getpid(), num);
            num++;

            sleep(5);

            int n2 = write(pipe2fd[1], &num, sizeof(int));
            if (n2 <= 0) {
                perror("child write to pipe error\n");
                exit(EXIT_FAILURE);
            }
            printf("[Child %d] [%d] Child finished hard work, writing back %d", gIndex, getpid(), num);

        }
        
    }

    //parent process
    while(1) {
        FD_ZERO(&readfds);
        FD_SET(STDIN_FILENO, &readfds);
        FD_SET(pipe2fd[0], &readfds);
        maxfd = max(pipe2fd[0], STDIN_FILENO);

        if (select(maxfd + 1, &readfds, NULL, NULL, NULL) < 0) {
            perror("select error");
            continue;
        }

        if (FD_ISSET(STDIN_FILENO, &readfds)) {
            //read data from stdin
            int n = read(STDIN_FILENO, buffer, BUFSIZE); //thelei akrivws oso einai to noumero alla tha to ftiaksw se 2h fash.

            buffer[n] = '\0';

            if (n <= 0) {
                perror("read from stdin error");
                exit(EXIT_FAILURE);
            }

            if (n > 0 && buffer[n-1] == '\n') {
                buffer[n-1] = '\n';
            }

            printf("Received input from stdin: %s\n", buffer);

            //write user's input to pipe1
            if (write(pipe1fd[1], buffer, BUFSIZE) < 0) {
                perror("write to pipe1 error\n");
                exit(EXIT_FAILURE);
            }
            printf("Assigned %s to child\n", buffer);

        }

        if (FD_ISSET(pipe2fd[0], &readfds)) {
            int n = read(pipe2fd[0], buffer, BUFSIZE);
            if (n <= 0) {
                perror("read from pipe error");
                exit(EXIT_FAILURE);
            }
            printf("Received message from child nolala: %s\n", buffer);
        }

        int n = write(pipe1fd[1], buffer, BUFSIZE);
        if (n < 0) {
            perror("write to pipe error");
            exit(EXIT_FAILURE);
        }
        printf("Writing to the child?\n");
        sleep(2);


        // write(pipe1fd[1], fds);
    }
    
}
