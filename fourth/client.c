#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <arpa/inet.h>

char *HOST;
char* serverHostname = "iot.dslab.pub.ds.open-cloud.xyz";
int PORT = 18080;
int DEBUG = 0;

int main(int argc, char **argv) { 
    if (argc > 6) { 
        printf("argc is larger than 4\n");
        exit(1);
    }

    if (argc == 6) { 
        if (strcmp(argv[1], "--host")){ 
            printf("Argv[1] is incorrect\n");
            exit(1);
        }

        if (strcmp(argv[3], "--port")){ 
            printf("Argv[3] is incorrect\n");
            exit(1);
        }

        if (strcmp(argv[5], "--debug")){ 
            printf("Argv[5] is incorrect\n");
            exit(1);
        }

        HOST = argv[2];
        PORT = atoi(argv[4]);
        DEBUG = 1;
    }

    if (argc == 5) { 
        if (strcmp(argv[1], "--host")){ 
            printf("Argv[1] is incorrect\n");
            exit(1);
        }

        if (strcmp(argv[3], "--port")){ 
            printf("Argv[3] is incorrect\n");
            exit(1);
        }

        HOST = argv[2];
        PORT = atoi(argv[4]);
    }

    if (argc == 4) { 
        if (strcmp(argv[1], "--host")){ 
            printf("Argv[1] is incorrect\n");
            exit(1);
        }

        if (strcmp(argv[3], "--port")){ 
            printf("Argv[3] is incorrect\n");
            exit(1);
        }

        printf("you need to specify which PORT you want to connect to!\n");
        exit(1);
    }

    if (argc == 3) { 
        if (strcmp(argv[1], "--host")){ 
            printf("Argv[1] is incorrect\n");
            exit(1);
        }

        HOST = argv[2];
    }

    if (argc == 2) { 
        if (strcmp(argv[1], "--host")){ 
            printf("Argv[1] is incorrect\n"); exit(1);
        }

        printf("you need to specify which HOST you want to connect to!\n");
        exit(1);
    }

    struct hostent *server_host = gethostbyname(serverHostname); //host info
    if (server_host == NULL) {
        perror("failed to resolve host name\n");
        exit(EXIT_FAILURE);
    }

    int network_socket;
    int max_fd = -1;

    network_socket = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in server_address;
    fd_set mask;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(PORT);
    memcpy(&server_address.sin_addr, server_host->h_addr, server_host->h_length);



    // /* Get info on host */
    // if ( (server_host=gethostbyaddr("iot.dslab.pub.ds.open-cloud.xyz", strlen("iot.dslab.pub.ds.open-cloud.xyz"),SOCK_STREAM)) == NULL ) {
    //     fprintf(stderr, "%s: unknown host %s\n", argv[0], argv[1]);
    //     exit(1);
    // }

    // if (inet_pton(AF_INET, serverHostname, &(server_address.sin_addr)) < 0) {
    //     perror("Invalid address/Address not supported");
    //     exit(EXIT_FAILURE);
    // }

    // bcopy((char*)server_host->h_addr, (char*)&server_address.sin_addr, server_host->h_length);
    int connection_status = connect(network_socket, (struct sockaddr *) &server_address, sizeof(server_address));
    printf("aaa\n");

    if (connection_status < 0) { 
        perror(0);
        exit(1);
    }

    while(1) { 
        printf("lol\n"); 
        FD_ZERO(&mask);

        FD_SET(network_socket, &mask);
        if (max_fd < network_socket) { 
            max_fd = network_socket;
        }

        FD_SET(STDIN_FILENO, &mask);
        if (max_fd < STDIN_FILENO) { 
            max_fd = STDIN_FILENO;
        }

        int select_ready = select(max_fd + 1, &mask, NULL, NULL, NULL);

        if(select_ready < 0) { 
            perror(0);
            exit(1);
        }
        char server_response[256];

        char buffer[101];
        int n_read;

        if (FD_ISSET(STDIN_FILENO, &mask)){ 
            n_read = read(STDIN_FILENO, buffer, 100);
            buffer[n_read] = '\0';

            if (n_read > 0 && buffer[n_read-1] == '\n') {
                buffer[n_read-1] = '\0';
            }

            if (DEBUG){ 
                printf("[DEBUG] sent %s\n", buffer);
            }

            printf("Got user input: %s\n", buffer);

            if (n_read >= 4 && strncmp(buffer, "exit", 4) == 0) {
                exit(0);
            }

            if (n_read >= 4 && strncmp(buffer, "help", 4) == 0) {
                printf("This is a help message\n");
            }

            if (n_read >= 3 && strncmp(buffer, "get", 3) == 0) {
                write(network_socket, &buffer, sizeof(buffer));
                // if (strlen(server_response) > 0){ 
                //     if (DEBUG){
                //         printf("read '%s'\n", server_response);
                //     }
                //
                //     char *token = strtok(server_response, " ");
                //     int values[4];
                //     int i = 0;
                //
                //     while (token != NULL && i < 4) { 
                //         values[i] = atoi(token);
                //         token = strtok(NULL, " ");
                //         i++;
                //     }
                //
                //     int third_value = values[2];
                //     int minutes = third_value / 100;
                //     int seconds = third_value % 100;
                //
                //     char forth_value[256];
                //     sprintf(forth_value, "%d", values[3]);
                //
                //     time_t rawtime;
                //     time(&rawtime);
                //     struct tm *info = localtime(&rawtime);
                //     printf("------------\n");
                //     printf("Latest event:\n");
                //     printf("interval (%d)\n", values[0]);
                //     printf("Temperature is: %d.%d\n", minutes, seconds);
                //     printf("Light level is: %d\n", values[1]);
                //     printf("Timestamp is: %s %s\n",forth_value, asctime(info));
                // }
            }

            if (n_read >= 7) { 
                write(network_socket, &buffer, sizeof(buffer));
            }

        }

        if (FD_ISSET(network_socket, &mask)){ 
            read(network_socket, &server_response, sizeof(server_response));
            if (strlen(server_response) > 0){ 
                if (DEBUG){
                    printf("read '%s'\n", server_response);
                }

                char *token = strtok(server_response, " ");
                int values[4];
                int i = 0;

                while (token != NULL && i < 4) { 
                    values[i] = atoi(token);
                    token = strtok(NULL, " ");
                    i++;
                }

                int third_value = values[2];
                int minutes = third_value / 100;
                int seconds = third_value % 100;

                char forth_value[256];
                sprintf(forth_value, "%d", values[3]);

                time_t rawtime;
                time(&rawtime);
                struct tm *info = localtime(&rawtime);
                printf("------------\n");
                printf("Latest event:\n");
                printf("interval (%d)\n", values[0]);
                printf("Temperature is: %d.%d\n", minutes, seconds);
                printf("Light level is: %d\n", values[1]);
                printf("Timestamp is: %s %s\n",forth_value, asctime(info));
            }
            printf("%s\n", server_response);            
            // recv(network_socket, &server_response, sizeof(server_response), 0);
        }


    }

    return 0;

}
