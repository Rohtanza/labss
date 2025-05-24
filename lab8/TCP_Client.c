#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#define MAX_LINE_LEN 256
#define PORT 2000

// Send a line to the server
void send_line(int sock, const char *line) {
    send(sock, line, strlen(line), 0);
}

// Receive a line from the server
void recv_line(int sock, char *buffer) {
    memset(buffer, 0, MAX_LINE_LEN);
    recv(sock, buffer, MAX_LINE_LEN, 0);
}

int main() {
    // Create socket
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("Could not create socket");
        return 1;
    }

    struct sockaddr_in server_addr = {
        .sin_family = AF_INET,
        .sin_port = htons(PORT),
        .sin_addr.s_addr = inet_addr("127.0.0.1")
    };

    // Connect to server
    if (connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Connection failed");
        return 1;
    }
    printf("Connected to server.\n");

    // Send name
    char buffer[MAX_LINE_LEN];
    printf("Enter your name: ");
    fgets(buffer, MAX_LINE_LEN, stdin);
    buffer[strcspn(buffer, "\n")] = 0;
    send_line(sock, buffer);

    // Receive authentication response
    recv_line(sock, buffer);
    printf("%s", buffer);
    if (strncmp(buffer, "Invalid name.", 13) == 0) {
        close(sock);
        return 0;
    }

    if (strncmp(buffer, "You have already voted.", 23) == 0) {
        // Menu for already voted users
        while (1) {
            printf("1. See results\n2. Exit\nEnter choice: ");
            fgets(buffer, MAX_LINE_LEN, stdin);
            buffer[strcspn(buffer, "\n")] = 0;
            send_line(sock, buffer);
            if (strncmp(buffer, "2", 1) == 0) {
                break;
            } else if (strncmp(buffer, "1", 1) == 0) {
                recv_line(sock, buffer);
                printf("%s", buffer);
            }
        }
    } else {
        // Voting process
        recv_line(sock, buffer); // Receive dish list
        printf("%s", buffer);
        while (1) {
            printf("Enter dish number and preference (1-5), or 'done': ");
            fgets(buffer, MAX_LINE_LEN, stdin);
            buffer[strcspn(buffer, "\n")] = 0;
            send_line(sock, buffer);
            if (strncmp(buffer, "done", 4) == 0) {
                break;
            }
            recv_line(sock, buffer);
            printf("%s", buffer);
        }

        // Post-voting menu
        recv_line(sock, buffer);
        printf("%s", buffer);
        while (1) {
            printf("Enter choice: ");
            fgets(buffer, MAX_LINE_LEN, stdin);
            buffer[strcspn(buffer, "\n")] = 0;
            send_line(sock, buffer);
            if (strncmp(buffer, "2", 1) == 0) {
                break;
            } else if (strncmp(buffer, "1", 1) == 0) {
                recv_line(sock, buffer);
                printf("%s", buffer);
            }
        }
    }
    close(sock);
    return 0;
}