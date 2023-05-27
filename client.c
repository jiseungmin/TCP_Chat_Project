#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>

#pragma comment(lib, "ws2_32.lib")

#define MAX_MESSAGE_LEN 200

int main() {
    WSADATA wsaData;
    SOCKET sock;
    struct sockaddr_in server;
    char message[MAX_MESSAGE_LEN], server_reply[MAX_MESSAGE_LEN];
    int recv_size;

    // Initialize Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        printf("Winsock 초기화 실패\n");
        return 1;
    }

    // Create a socket
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == INVALID_SOCKET) {
        printf("소켓 생성 실패\n");
        return 1;
    }

    server.sin_family = AF_INET;
    server.sin_port = htons(9999);
    if (inet_pton(AF_INET, "ip주소입력", &(server.sin_addr)) <= 0) {
        printf("유효하지 않은 IP 주소\n");
        return 1;
    }

    // Connect to the server
    if (connect(sock, (struct sockaddr*)&server, sizeof(server)) < 0) {
        printf("연결 실패\n");
        return 1;
    }

    printf("서버에 연결되었습니다.\n");

    // Start the chat
    while (1) {
        printf("메시지 입력: ");
        fgets(message, MAX_MESSAGE_LEN, stdin);
        message[strcspn(message, "\n")] = '\0';

        // Send the message to the server
        if (send(sock, message, strlen(message), 0) < 0) {
            printf("메시지 전송 실패\n");
            return 1;
        }

        // Receive response from the server
        memset(server_reply, 0, sizeof(server_reply));
        recv_size = recv(sock, server_reply, MAX_MESSAGE_LEN, 0);
        if (recv_size == SOCKET_ERROR) {
            printf("수신 실패\n");
            return 1;
        }
        else if (recv_size == 0) {
            printf("서버 연결이 종료되었습니다.\n");
            break;
        }

        printf("서버 응답: %s\n", server_reply);

        // Set the exit condition
        if (strcmp(message, "bye") == 0)
            break;

        memset(message, 0, sizeof(message));
    }

    // Clean up the socket and Winsock
    closesocket(sock);
    WSACleanup();

    return 0;
}
