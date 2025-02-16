
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>


#define BUFFER_SIZE 1024


void tcp_send(void *data)
{
    ESP_LOGI(TCP_CLIENT, "tcp_send() called");
    int client_fd;
    struct sockaddr_in client_addr;
    static char buf[BUFFER_SIZE];
    socklen_t client_size = sizeof(client_addr);

    memset(buf, 0, BUFFER_SIZE);
    buf[BUFFER_SIZE - 1] = '\0';

    strncpy(buf, "hello world!", BUFFER_SIZE - 1);

    printf("tcp_send: %s\n", buf);

    client_fd = socket(AF_INET, SOCK_STREAM, 0);
    if(client_fd == -1)
        ESP_LOGE(TCP_CLIENT, "socket creation error");
    
    client_addr.sin_family = AF_INET;
    client_addr.sin_port = htons(4242);

    // PC ip from esp32 AP is "192.168.4.2" ... not really clean bu good enough for an example.
    inet_pton(AF_INET, "192.168.4.2", &client_addr.sin_addr);

    if(connect(client_fd, (struct sockaddr *)&client_addr, client_size) < 0)
    {
        ESP_LOGE(TCP_CLIENT, "connect error");
        close(client_fd);
    }

    ESP_LOGI(TCP_CLIENT, "connected");
    
    ssize_t sent_size = send(client_fd, buf, BUFFER_SIZE, 0);
    if(sent_size > 0)
        ESP_LOGI(TCP_CLIENT, "send %i bytes", sent_size);

    memset(buf, 0, BUFFER_SIZE);
    buf[BUFFER_SIZE - 1] = '\0';

    int byte_rcvd = recv(client_fd, buf, BUFFER_SIZE-1, 0);
    
    if(byte_rcvd == -1)
        printf("error %s\n", strerror(errno));

    printf("recv message : %s\n", buf);

    vTaskDelete(NULL);
}
