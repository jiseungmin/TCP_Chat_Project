#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <winsock2.h>
#include <windows.h>

#pragma comment(lib, "ws2_32.lib")

#define MAX_MESSAGE_LEN 200

int openFileExplorer(const char* path) {
    char command[256];

#ifdef _WIN32
    sprintf(command, "explorer.exe /select,\"%s\"", path);
    return system(command);
#else
    printf("���� Ž���⸦ ���� ���� �ü�� ������ �ʿ��մϴ�.\n");
    return -1;
#endif
}

void receiveDataThread(SOCKET sock) {
    char server_reply[MAX_MESSAGE_LEN];
    int recv_size;

    while (1) {
        memset(server_reply, 0, sizeof(server_reply));
        recv_size = recv(sock, server_reply, MAX_MESSAGE_LEN, 0);

        if (recv_size == SOCKET_ERROR) {
            printf("���� ����\n");
            break;
        }
        else if (recv_size == 0) {
            printf("���� ������ ����Ǿ����ϴ�.\n");
            break;
        }
        printf("%s\n", server_reply);
    }
}

int main() {
    WSADATA wsaData;
    SOCKET sock;
    struct sockaddr_in server;
    char message[MAX_MESSAGE_LEN];
    HANDLE receiveThread;
    DWORD threadID;

    // Initialize Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        printf("Winsock �ʱ�ȭ ����\n");
        return 1;
    }

    // Create a socket
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == INVALID_SOCKET) {
        printf("���� ���� ����\n");
        return 1;
    }

    server.sin_family = AF_INET;

    // Start the chat
    while (1) {
        printf("****************************************************************************************\n\n\n");
        printf("\t\t\t14�� TCP Socket Communication Chat Program- V 1.0.0\n\n\n");
        printf("****************************************************************************************\n\n\n");
        printf("1. �г��� �Է�\n\n");
        printf("2. ä�� ���� ����\n\n");
        printf("3. ä�� �̷� �ؽ�Ʈ ����Ž���� ����\n\n");
        printf("4. ���α׷� ����\n\n");
        printf("****************************************************************************************\n\n\n");
        int choice;
        scanf("%d", &choice);
        getchar(); // �Է� ���� ����

        switch (choice) {
        case 1:
            // �г����� �Է��մϴ�.
            printf("����Ͻ� �г����� �Է��ϼ���: ");
            char nickname[100];
            fgets(nickname, sizeof(nickname), stdin);
            nickname[strcspn(nickname, "\n")] = '\0';  // ���� ���� ����

            printf("Nickname: %s\n", nickname);
            break;

        case 2:
            // ä�� ������ ���ϴ�.
            if (strlen(nickname) > 0) {
                // ���� IP �ּҸ� �Է��մϴ�.
                printf("���� IP �ּҸ� �Է��ϼ���: ");
                char server_ip[100];
                fgets(server_ip, sizeof(server_ip), stdin);
                server_ip[strcspn(server_ip, "\n")] = '\0';  // ���� ���� ����

                // Convert IPv4 address
                server.sin_addr.s_addr = inet_addr(server_ip);
                if (server.sin_addr.s_addr == INADDR_NONE) {
                    printf("��ȿ���� ���� IP �ּ�\n");
                    return 1;
                }

                server.sin_port = htons(9999);

                // Connect to the server
                if (connect(sock, (struct sockaddr*)&server, sizeof(server)) < 0) {
                    printf("���� ����\n");
                    return 1;
                }

                printf("������ ����Ǿ����ϴ�.\n");
                printf("�޽��� �Է� (�������� q�� �Է��ϼ���): \n");

                // Create a thread to receive data asynchronously
                receiveThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)receiveDataThread, (LPVOID)sock, 0, &threadID);

                while (1) {
                    fgets(message, MAX_MESSAGE_LEN, stdin);
                    message[strcspn(message, "\n")] = '\0';  // ���� ���� ����

                    if (strcmp(message, "q") == 0) {
                        // Send the "disconnect" message along with the nickname to the server
                        char disconnect_message[MAX_MESSAGE_LEN + 100];
                        sprintf(disconnect_message, "%s���� ä�ù濡 �����̽��ϴ�.", nickname);
                        if (send(sock, disconnect_message, strlen(disconnect_message), 0) < 0) {
                            printf("�޽��� ���� ����\n");
                            return 1;
                        }
                        // Clean up the socket and Winsock
                        closesocket(sock);
                        WSACleanup();
                        break;
                    }

                    // Save chat history to a file
                    FILE* file = fopen("chat_history.txt", "a");
                    if (file != NULL) {
                        fprintf(file, "[%s] %s\n", nickname, message);
                        fclose(file);
                    }
                    else {
                        printf("ä�� ����� �����ϴ� ���� ������ �߻��߽��ϴ�.\n");
                    }

                    // Send the message to the server
                    char formatted_message[MAX_MESSAGE_LEN + 100];
                    sprintf(formatted_message, "%s: %s", nickname, message);
                    fprintf(file, "[%s] %s\n", formatted_message);
                    fclose(file);
                    if (send(sock, formatted_message, strlen(formatted_message), 0) < 0) {
                        printf("�޽��� ���� ����\n");
                        return 1;
                    }
                }

                // Wait for the receive thread to exit
                WaitForSingleObject(receiveThread, INFINITE);
                CloseHandle(receiveThread);
            }
            else {
                printf("�г����� ���� �Է��ϼ���.\n");
            }
            break;

        case 3:
            // ä�� �̷� �ؽ�Ʈ ���� Ž���⸦ ���ϴ�.
            if (strlen(nickname) > 0) {
                // ä�� �̷� �ؽ�Ʈ ���� ��θ� �����ϼ���.
                const char* filePath = "chat_history.txt";

                // ���� Ž���⸦ ���ϴ�.
                int result = openFileExplorer(filePath);
                if (result != 0) {
                    printf("���� Ž���⸦ �� �� �����ϴ�. ���� �ڵ�: %d\n", result);
                }
            }
            else {
                printf("�г����� ���� �Է��ϼ���.\n");
            }
            break;

        case 4:
            // ���α׷��� �����մϴ�.
            printf("���α׷��� �����մϴ�.\n");
            return 0;

        default:
            // �߸��� ������ ��� ���� ȭ������ ���ư��ϴ�.
            printf("�߸��� �����Դϴ�. ���� �޴��� ���ư��ϴ�.\n");
            continue;
        }
    }

    return 0;
}
