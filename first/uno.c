#include <stdio.h>
#include <fcntl.h>
// #include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <string.h>
#include <sys/stat.h>
#include <stdbool.h>


bool file_exists (char *filename) {
	struct stat buffer;
    return (stat (filename, &buffer) == 0);//stat=0 Στο struct stat buffer καταλαμβάνω(reserve) χώρο μνήμης. 
}

int main(int argc, char *argv[]) {


    
	if ( argc == 2 ) { // okay
		if ( file_exists(argv[1]) ) {
            printf("Error: %s already exists\n", argv[1]);
            exit(1);
        }
		if ( strcmp("--help", argv[1]) == 0 ) {
            printf("Usage: ./a.out filename\n");
            exit(1);
        }
	}
	else {
		printf("Usage: ./a.out filename\n");
        exit(1);
	}
    
    int fifi = open(argv[1], O_CREAT | O_APPEND | O_WRONLY, 0644); //returns a non-negative integer: file descriptor(index)
    char buf[50];
    int maxlen = sizeof buf;

    int status;
    pid_t child;
    child = fork();

    if (child < 0) {
        exit(0);
    }
    
    if (child == 0) { //child process
        int a = snprintf(buf, maxlen, "[CHILD] getpid()= %d getppid()= %d \n", getpid(), getppid());
        if ( a > 0 ) write(fifi, buf, maxlen);
    }

    else { //parent process
        wait(&status);
        int b = snprintf(buf, maxlen, "[PARENT] getpid()= %d getppid()= %d \n", getpid(), getppid());
        if ( b > 0 ) write(fifi, buf, maxlen);
    }
    close(fifi);
    return 0;
}
