#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <sys/epoll.h>

#define MAX_EVENTS 10
#define BUF_SIZE 1024
#define PORT 12345

void *client_handler(void *arg) {
    int clnt_sock = *(int *)arg;
    char buffer[BUF_SIZE];
    int str_len;

    while ((str_len = read(clnt_sock, buffer, BUF_SIZE)) != 0) {
        if (str_len == -1) {
            perror("read error");
            break;
        }

        buffer[str_len] = '\0';
        printf("Received from client: %s", buffer);
        write(clnt_sock, buffer, str_len);
    }

    close(clnt_sock);
    return NULL;
}

void error_handling(const char *message) {
    perror(message);
    exit(1);
}

int main() {
    int serv_sock, clnt_sock;
    struct sockaddr_in serv_addr, clnt_addr;
    socklen_t addr_size;
    pthread_t thread_id;

    int epfd, event_cnt;
    struct epoll_event event, events[MAX_EVENTS];

    serv_sock = socket(PF_INET, SOCK_STREAM, 0);
    if (serv_sock == -1)
        error_handling("socket() error");

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(PORT);

    if (bind(serv_sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == -1)
        error_handling("bind() error");

    if (listen(serv_sock, 5) == -1)
        error_handling("listen() error");

    epfd = epoll_create(50);
    if (epfd == -1)
        error_handling("epoll_create() error");

    event.events = EPOLLIN;
    event.data.fd = serv_sock;

    if (epoll_ctl(epfd, EPOLL_CTL_ADD, serv_sock, &event) == -1)
        error_handling("epoll_ctl() error");

    printf("Server is running on port %d\n", PORT);

    while (1) {
        event_cnt = epoll_wait(epfd, events, MAX_EVENTS, -1);
        if (event_cnt == -1)
            error_handling("epoll_wait() error");

        for (int i = 0; i < event_cnt; i++) {
            if (events[i].data.fd == serv_sock) {
                addr_size = sizeof(clnt_addr);
                clnt_sock = accept(serv_sock, (struct sockaddr *)&clnt_addr, &addr_size);
                if (clnt_sock == -1)
                    error_handling("accept() error");

                printf("Connected client: %d\n", clnt_sock);

                if (pthread_create(&thread_id, NULL, client_handler, (void *)&clnt_sock) != 0)
                    error_handling("pthread_create() error");
            }
        }
    }

    close(serv_sock);
    close(epfd);
    return 0;
}
