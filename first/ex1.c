#include <stdio.h>
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
                                           //Ελέγχω αν αυτός ο χώρος μνήμης έχει καταληφθει από το filename. Τοτε stat=0, file exists
                                           //else stat=-1. 
}

int main(int argc, char *argv[]) {
	char c;

	FILE *fifi;
	
	// if ( argc == 1 ) { // ./a.out only
	// 	printf("Usage: ./a.out filename\n");
	// 	exit(1);
	// }	

	if ( argc == 2 ) { // okay
		if ( file_exists(argv[1]) ) printf("Error: %s already exists\n", argv[1]);
		if ( strcmp("--help", argv[1]) == 0 ) printf("Usage: ./a.out filename\n");
		fifi = fopen(argv[1], "w");
	}
	else {
		printf("Usage: ./a.out filename\n");
        exit(1);
	}
	
	int status;
	pid_t child;
	child = fork();

	if ( child < 0 ) {
		exit(0);
	}

	if ( child == 0 ) { //child process
		fprintf(fifi, "[CHILD] getpid()= %d getppid()= %d\n", getpid(), getppid());
		exit(0);
	}
	else { //parent process
		wait(&status);
		fprintf(fifi, "[PARENT] getpid()= %d getppid()= %d\n", getpid(), getppid());
	}

	fclose(fifi); return 0; }
