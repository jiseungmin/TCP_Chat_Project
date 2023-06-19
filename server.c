#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <winsock2.h>
#include <process.h>

#pragma comment(lib, "ws2_32.lib")

#define MAX_CLIENTS 10
#define MAX_BUFFER_SIZE 1024

typedef struct {
    SOCKET clientSocket;
} ClientData;

// �߰�: Ŭ���̾�Ʈ�� ���� ����� ���� ���� �迭
SOCKET clientSockets[MAX_CLIENTS] = { 0 };

void HandleClient(void* clientData);

int main() {
    WSADATA wsaData;
    SOCKET serverSocket;
    SOCKADDR_IN serverAddr;
    int addrLen = sizeof(serverAddr);
    int clientCount = 0;
    ClientData clientData[MAX_CLIENTS];
    HANDLE clientThreads[MAX_CLIENTS];

    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        printf("WSAStartup failed.\n");
        return 1;
    }

    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == INVALID_SOCKET) {
        printf("Failed to create server socket.\n");
        return 1;
    }

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(9999);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    if (bind(serverSocket, (SOCKADDR*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        printf("Failed to bind.\n");
        closesocket(serverSocket);
        return 1;
    }

    if (listen(serverSocket, SOMAXCONN) == SOCKET_ERROR) {
        printf("LISTEN failed.\n");
        closesocket(serverSocket);
        return 1;
    }

    printf("���� �۵� ���Դϴ�. Ŭ���̾�Ʈ�� ������ ��ٸ��� ���Դϴ�..\n");

    while (1) {
        if (clientCount < MAX_CLIENTS) {
            clientData[clientCount].clientSocket = accept(serverSocket, (SOCKADDR*)&serverAddr, &addrLen);
            if (clientData[clientCount].clientSocket == INVALID_SOCKET) {
                printf("Failed to accept client connection.\n");
                continue;
            }

            printf("Client connected: %s:%d\n", inet_ntoa(serverAddr.sin_addr), ntohs(serverAddr.sin_port));

            // �߰�: ����� Ŭ���̾�Ʈ ������ �迭�� ����
            clientSockets[clientCount] = clientData[clientCount].clientSocket;

            clientThreads[clientCount] = (HANDLE)_beginthreadex(NULL, 0, (_beginthreadex_proc_type)HandleClient, (void*)&clientData[clientCount], 0, NULL);
            if (clientThreads[clientCount] == NULL) {
                printf("Failed to create client thread.\n");
                closesocket(clientData[clientCount].clientSocket);
            }

            clientCount++;
        }
    }

    for (int i = 0; i < clientCount; i++) {
        WaitForSingleObject(clientThreads[i], INFINITE);
        CloseHandle(clientThreads[i]);
        closesocket(clientData[i].clientSocket);
    }

    closesocket(serverSocket);
    WSACleanup();

    return 0;
}

void HandleClient(void* clientData) {
    ClientData* data = (ClientData*)clientData;
    SOCKET clientSocket = data->clientSocket;

    FILE* fp;
    char buffer[MAX_BUFFER_SIZE];
    int bytesRead;

    while (1) {
        bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);
        if (bytesRead <= 0) {
            break;
        }

        buffer[bytesRead] = '\0';

        printf("%s\n", buffer);
        fp = fopen("chat_history.txt", "a");
        if (fp != NULL) {
            fprintf(fp, "%s\n", buffer);
            fclose(fp);
        }
        else {
            printf("ä�� ����� �����ϴ� ���� ������ �߻��߽��ϴ�.\n");
        }
        // �߰�: ���� ä�� �޽����� �ٸ� Ŭ���̾�Ʈ�鿡�� ����
        for (int i = 0; i < MAX_CLIENTS; i++) {
            if (clientSockets[i] != INVALID_SOCKET && clientSockets[i] != clientSocket) {
                send(clientSockets[i], buffer, bytesRead, 0);
            }
        }
    }

    // Ŭ���̾�Ʈ ������ �迭���� ����
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (clientSockets[i] == clientSocket) {
            clientSockets[i] = INVALID_SOCKET;
            break;
        }
    }
    closesocket(clientSocket);
    _endthreadex(0);
}
