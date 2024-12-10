#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>

#define PORT 8080
#define BUFFER_SIZE 1024

void handle_client(int client_socket);
void execute_cgi(const char *script_path, const char *query_string, int client_socket);

int main() {
    int server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_addr_len = sizeof(client_addr);

    // Create socket
    if ((server_socket = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Socket failed");
        exit(EXIT_FAILURE);
    }

    // Configure server address
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    // Bind socket
    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind failed");
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    // Listen for connections
    if (listen(server_socket, 5) < 0) {
        perror("Listen failed");
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    printf("Server listening on port %d\n", PORT);

    // Accept and handle client connections
    while ((client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &client_addr_len)) >= 0) {
        handle_client(client_socket);
        close(client_socket);
    }

    close(server_socket);
    return 0;
}

void handle_client(int client_socket) {
    char buffer[BUFFER_SIZE];
    char method[16], path[256], protocol[16];
    char *query_string = NULL;

    // Read the HTTP request
    int received = recv(client_socket, buffer, BUFFER_SIZE - 1, 0);
    if (received < 0) {
        perror("Recv failed");
        return;
    }
    buffer[received] = '\0';

    // Parse the HTTP request line
    sscanf(buffer, "%s %s %s", method, path, protocol);

    // Check for query string in the path
    char *question_mark = strchr(path, '?');
    if (question_mark != NULL) {
        *question_mark = '\0';
        query_string = question_mark + 1;
    }

    // Handle GET and POST methods
    if (strcmp(method, "GET") == 0) {
        if (strstr(path, "cgi-bin/") != NULL) {
            execute_cgi(path + 1, query_string, client_socket);
        } else {
            // Send a simple HTTP response
            char response[] =
                "HTTP/1.1 200 OK\r\n"
                "Content-Type: text/plain\r\n\r\n"
                "Hello, World! GET Request Received.";
            send(client_socket, response, strlen(response), 0);
        }
    } else if (strcmp(method, "POST") == 0) {
        char *body = strstr(buffer, "\r\n\r\n");
        if (body != NULL) {
            body += 4; // Skip the \r\n\r\n
            if (strstr(path, "cgi-bin/") != NULL) {
                execute_cgi(path + 1, body, client_socket);
            } else {
                char response[] =
                    "HTTP/1.1 200 OK\r\n"
                    "Content-Type: text/plain\r\n\r\n"
                    "Hello, World! POST Data Received.";
                send(client_socket, response, strlen(response), 0);
            }
        }
    } else {
        char response[] =
            "HTTP/1.1 405 Method Not Allowed\r\n"
            "Content-Type: text/plain\r\n\r\n"
            "Method Not Allowed.";
        send(client_socket, response, strlen(response), 0);
    }
}

void execute_cgi(const char *script_path, const char *query_string, int client_socket) {
    char response[BUFFER_SIZE];
    FILE *fp;
    snprintf(response, BUFFER_SIZE,
             "HTTP/1.1 200 OK\r\n"
             "Content-Type: text/plain\r\n\r\n");
    send(client_socket, response, strlen(response), 0);
    // Set environment variables
    if (query_string != NULL) {
        setenv("QUERY_STRING", query_string, 1);
    }
    // Execute CGI script
    fp = popen(script_path, "r");
    if (fp == NULL) {
        perror("Failed to execute CGI script");
        return;
    }
    while (fgets(response, BUFFER_SIZE, fp) != NULL) {
        send(client_socket, response, strlen(response), 0);
    }
    pclose(fp);
}
