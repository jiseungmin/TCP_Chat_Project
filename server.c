#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <winsock2.h>

#pragma comment(lib, "ws2_32.lib") // Ư�� ���̺귯�� ������ �ڵ����� ��ũ�ϵ��� �����Ϸ��� �����ϴ� ������ �մϴ�.

#define MAX_CLIENTS 10
#define MAX_BUFFER_SIZE 1024

void HandleClient(SOCKET clientSocket);

int main() {
    WSADATA wsaData; // window ���� ������ ���� ���� ����
    SOCKET serverSocket, clientSocket; // ��������, Ŭ���̾�Ʈ ����
    SOCKADDR_IN serverAddr, clientAddr; // ������ Ŭ���̾�Ʈ �����ּ�
    int addrLen = sizeof(clientAddr); //�ּ� ũ��

    // Initialize Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) { //  winsock ���̺귯�� �ʱ�ȭ
        printf("���ῡ ���� �ϼ̽��ϴ�.(WSAStartup)\n");
        return 1;
    }

    // Create a socket
    serverSocket = socket(AF_INET, SOCK_STREAM, 0); //AF_INET :IPv4 �ּ�ü��, SOCK_STREAM TCP�������� ���, 0 �������� �ڵ� ����
    if (serverSocket == INVALID_SOCKET) { //INVALID_SOCKET ��ȿ���� ���� ������ ��Ÿ���� ��ũ�� ���
        printf("���ῡ ���� �ϼ̽��ϴ�.(serverSocket)\n");
        return 1;
    }

    // Set server address information
    serverAddr.sin_family = AF_INET; // IPv4 �ּ� ü�踦 ����ϰڴ�.
    serverAddr.sin_port = htons(9999); // 9999��Ʈ�� ��ȯ�Ͽ� ����
    serverAddr.sin_addr.s_addr = INADDR_ANY; // ��� ��Ʈ��ũ �������̽����� ������ ������ ���� 

    // Bind the socket to the server address
    //���� ������ Ư�� IP �ּҿ� ��Ʈ��ȣ�� �����Ͽ� �ش� �ּҿ� ��Ʈ���� Ŭ���̾�Ʈ�� ������ �����Ҽ� �ֵ��� �ϴ°�
    if (bind(serverSocket, (SOCKADDR*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) // �������ϰ� ���� �ּ� ���ε�
    {
        printf("Bind ����.\n");
        closesocket(serverSocket); // ���� ���� �ݱ�
        return 1;
    }

    // Listen for incoming connections
    // listen �Լ��� ���� ������ ���� Ŭ���̾�Ʈ�� ���� ��û�� �����ϱ� ���� ������ ���� ��� ���·� �����ϴ� ����
    if (listen(serverSocket, SOMAXCONN) == SOCKET_ERROR) { // ���� ������ Ŭ���̾�Ʈ�� ���� ��û�� ���� �� �ִ� ���°� ��
        printf("LISTEN ����.\n");
        closesocket(serverSocket);// ���� ���� �ݱ�
        return 1;
    }

    printf("���� �۵����Դϴ�. Ŭ���̾�Ʈ�� ������ ��ٸ��� ���Դϴ�..\n");

    while (1) {
        // Accept a client connection
        clientSocket = accept(serverSocket, (SOCKADDR*)&clientAddr, &addrLen); //Ŭ���̾�Ʈ�� ���� ��û�� ��ٸ���, ���� ��û�� �����Ǹ� ���ο� ������ clientSocket�� ����
        if (clientSocket == INVALID_SOCKET) {
            printf("Ŭ���̾�Ʈ�� ��� ����.\n");
            continue;
        }

        printf("Client connected: %s:%d\n", inet_ntoa(clientAddr.sin_addr), ntohs(clientAddr.sin_port)); 

        // Handle the client connection
        HandleClient(clientSocket); // Ŭ���̾�Ʈ���� ���� ������ ��� �Լ�

        printf("Client disconnected: %s:%d\n", inet_ntoa(clientAddr.sin_addr), ntohs(clientAddr.sin_port));

        // Close the client socket
        closesocket(clientSocket); // Ŭ���̾�Ʈ ���� �ݱ�
    }

    // Cleanup
    closesocket(serverSocket); // ���� ���� �ݱ�
    WSACleanup(); // ���̺귯�� ����� �ڿ� ����

    return 0;
}

void HandleClient(SOCKET clientSocket) {
    FILE* fp;
    char buffer[MAX_BUFFER_SIZE];
    int bytesRead;

    while (1) {
        // Receive data from client
        bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0); // Ŭ���̾�Ʈ�κ��� �����͸� �޴´�
        if (bytesRead <= 0) {
            break;
        }

        // Null-terminate the received data
        buffer[bytesRead] = '\0'; // ���ŵ� ������ ��ó��

        printf("%s\n", buffer); // ���ŵ� ������ ���
        fp = fopen("chat_history.txt", "a");
        if (fp != NULL) {
            fprintf(fp, "%s\n", buffer);
            fclose(fp);
        }
        else {
            printf("ä�� ����� �����ϴ� ���� ������ �߻��߽��ϴ�.\n");
        };

        // Echo the received data back to the client
        send(clientSocket, buffer, bytesRead, 0); // Ŭ���̾�Ʈ���� �����͸� �ٽ� ������
    }
}
