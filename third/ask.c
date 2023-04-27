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
    fd_set writefds;
    fd_set readfds;
    int maxfd;

    if(pipe(pipe1fd) < 0 || pipe(pipe2fd) < 0) {
        perror("pipe error");
        exit(EXIT_FAILURE);
    }

    pid_t pid = fork();

    if (pid < 0) {
        perror("fork error");
        exit(EXIT_FAILURE);
    }

    else if (pid == 0){ //child process
        
    }

    else { //parent process
        while(1) {
            FD_ZERO(&writefds);
            FD_ZERO(&readfds);
            FD_SET(STDIN_FILENO, &readfds);
            FD_SET(pipe1fd[1], &writefds);
            FD_SET(pipe2fd[0], &readfds);
            maxfd = max(pipe1fd[1], max(pipe2fd[0], STDIN_FILENO));

            if (select(maxfd + 1, &readfds, &writefds, NULL, NULL) < 0) {
                perror("select error");
                exit(EXIT_FAILURE);
            }

            if (FD_ISSET(STDIN_FILENO, &readfds)) {
                //read data from stdin
                int n = read(STDIN_FILENO, buffer, BUFSIZE); //thelei akrivws oso einai to noumero alla tha to ftiaksw se 2h fash.
                if (n < 0) {
                    perror("read from stdin error");
                    exit(EXIT_FAILURE);
                }
                printf("Received input from stdin: %s\n", buffer);
            }
            
            if (FD_ISSET(pipe2fd[0], &readfds)) {
                int n = read(pipe2fd[0], buffer, BUFSIZE);
                if (n < 0) {
                    perror("read from pipe error");
                    exit(EXIT_FAILURE);
                }
                printf("Received message from child nolala: %s\n", buffer);
            }

            if (FD_ISSET(pipe1fd[1], &writefds)) {
                int n = write(pipe1fd[1], buffer, BUFSIZE);
                if (n < 0) {
                    perror("read from pipe error");
                    exit(EXIT_FAILURE);
                }
                printf("Writing to the child?");
            }

        // write(pipe1fd[1], fds);
        }
    }
}
