#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>
 
void handler_sigalrm(int sig){
    printf("Inside handler function\n");
    sleep(3);
    alarm(2);
    printf("just set an alarm inside the handler\n");
}
 
int main(){

    struct sigaction sal = {0};
    sal.sa_handler = &handler_sigalrm;
    sal.sa_flags = SA_RESTART;
    sigaction(SIGALRM, &sal, NULL); 

    alarm(2); 

    for(int i=1;;i++){
        printf("%d : Inside main function\n",i);
        pause();
        // sleep(1);  
    }

    return 0;
}
