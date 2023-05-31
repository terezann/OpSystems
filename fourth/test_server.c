#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <time.h>

#define PORT 12345

int getSystemTemperature()
{
    int temperature = 0;
    FILE *fp;
    char path[1024];

    // Execute "sensors" command to get system temperature
    fp = popen("sensors | grep -m1 'temp1' | awk '{print $2}'", "r");
    if (fp == NULL)
    {
        printf("Failed to execute sensors command.\n");
        return temperature;
    }

    // Read the temperature value
    if (fgets(path, sizeof(path), fp) != NULL)
    {
        sscanf(path, "%d", &temperature);
    }

    // Close the command output stream
    pclose(fp);

    return temperature;
}

int getSystemBrightness()
{
    int brightness = 0;
    FILE *fp;
    char path[1024];

    // Execute "xbacklight" command to get system brightness
    fp = popen("xbacklight -get", "r");
    if (fp == NULL)
    {
        printf("Failed to execute xbacklight command.\n");
        return brightness;
    }

    // Read the brightness value
    if (fgets(path, sizeof(path), fp) != NULL)
    {
        sscanf(path, "%d", &brightness);
    }

    // Close the command output stream
    pclose(fp);

    return brightness;
}

int main()
{
    int server_fd, new_socket, valread;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    char buffer[1024] = {0};
    char response[1024] = {0};

    // Create socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // Set socket options to allow multiple connections
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)))
    {
        perror("setsockopt failed");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // Bind the socket to the specified port
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    // Listen for incoming connections
    if (listen(server_fd, 1) < 0)
    {
        perror("listen failed");
        exit(EXIT_FAILURE);
    }

    printf("Server listening on port %d\n", PORT);

    while (1)
    {
        // Accept a new client connection
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0)
        {
            perror("accept failed");
            exit(EXIT_FAILURE);
        }

        printf("Client connected\n");

        // Receive data from the client
        valread = read(new_socket, buffer, 1024);

        if (strcmp(buffer, "get") == 0)
        {
            // Generate event information
            int event_type = rand() % 5; // Random event type between 0 and 4
            int brightness = getSystemBrightness();
            int temperature = getSystemTemperature();
            time_t current_time = time(NULL);

            // Prepare the response message
            snprintf(response, sizeof(response), "%d %03d %04d %ld", event_type, brightness, temperature, current_time);

            // Send the response to the client
            send(new_socket, response, strlen(response), 0);
            printf("Response sent: %s\n", response);
        }

        // Close the client socket
        close(new_socket);
    }

    return 0;
}

