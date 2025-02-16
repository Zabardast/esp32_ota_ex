#include <stdio.h>
#include <errno.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 4242
#define BACKLOG 5
#define BUFFER_SIZE 1024

int main()
{
    int server_fd, client_fd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_addr_len = sizeof(client_addr);
    char sbuffer[BUFFER_SIZE];
    char rbuffer[BUFFER_SIZE];
    const char *hello_msg = "I am not world. I am server1. \n";

    memset(rbuffer, 0, BUFFER_SIZE);

    // Step 1: Create socket
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }
    printf("Socket created successfully\n");

    // Step 2: Bind socket to a port
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY; // Accept connections from any address
    server_addr.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("Bind failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }
    printf("Bind successful\n");

    // Step 3: Listen for incoming connections
    if (listen(server_fd, BACKLOG) == -1) {
        perror("Listen failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }
    printf("Listening on port %d...\n", PORT);

    // Step 4: Accept a connection
    client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &client_addr_len);
    if (client_fd == -1) {
        perror("Accept failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }
    printf("Connection accepted from %s:%d\n",
           inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

    // Step 5: Communicate with the client
    int byte_rcvd = recv(client_fd, rbuffer, BUFFER_SIZE-1, 0);
    printf("byte_rcvd : %i\n",byte_rcvd);
    if(byte_rcvd == -1)
        printf("error %s\n", strerror(errno));
    // else
        // rbuffer[byte_rcvd] = '\0';
    printf("Recv message: %s\n", rbuffer);

    // then send message back to client
    strcpy(sbuffer, hello_msg);
    send(client_fd, sbuffer, strlen(sbuffer), 0);
    printf("Sent message: %s\n", sbuffer);

    // Step 6: Close the connection
    close(client_fd);
    printf("Connection closed\n");

    // Step 7: Close the server socket
    close(server_fd);
    printf("Server socket closed\n");

    return 0;

}