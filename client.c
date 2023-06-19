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
    printf("파일 탐색기를 열기 위한 운영체제 지원이 필요합니다.\n");
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
            printf("수신 실패\n");
            break;
        }
        else if (recv_size == 0) {
            printf("서버 연결이 종료되었습니다.\n");
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

    // Start the chat
    while (1) {
        printf("****************************************************************************************\n\n\n");
        printf("\t\t\t14팀 TCP Socket Communication Chat Program- V 1.0.0\n\n\n");
        printf("****************************************************************************************\n\n\n");
        printf("1. 닉네임 입력\n\n");
        printf("2. 채팅 서버 들어가기\n\n");
        printf("3. 채팅 이력 텍스트 파일탐색기 열기\n\n");
        printf("4. 프로그램 종료\n\n");
        printf("****************************************************************************************\n\n\n");
        int choice;
        scanf("%d", &choice);
        getchar(); // 입력 버퍼 비우기

        switch (choice) {
        case 1:
            // 닉네임을 입력합니다.
            printf("사용하실 닉네임을 입력하세요: ");
            char nickname[100];
            fgets(nickname, sizeof(nickname), stdin);
            nickname[strcspn(nickname, "\n")] = '\0';  // 개행 문자 제거

            printf("Nickname: %s\n", nickname);
            break;

        case 2:
            // 채팅 서버로 들어갑니다.
            if (strlen(nickname) > 0) {
                // 서버 IP 주소를 입력합니다.
                printf("서버 IP 주소를 입력하세요: ");
                char server_ip[100];
                fgets(server_ip, sizeof(server_ip), stdin);
                server_ip[strcspn(server_ip, "\n")] = '\0';  // 개행 문자 제거

                // Convert IPv4 address
                server.sin_addr.s_addr = inet_addr(server_ip);
                if (server.sin_addr.s_addr == INADDR_NONE) {
                    printf("유효하지 않은 IP 주소\n");
                    return 1;
                }

                server.sin_port = htons(9999);

                // Connect to the server
                if (connect(sock, (struct sockaddr*)&server, sizeof(server)) < 0) {
                    printf("연결 실패\n");
                    return 1;
                }

                printf("서버에 연결되었습니다.\n");
                printf("메시지 입력 (끝내려면 q를 입력하세요): \n");

                // Create a thread to receive data asynchronously
                receiveThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)receiveDataThread, (LPVOID)sock, 0, &threadID);

                while (1) {
                    fgets(message, MAX_MESSAGE_LEN, stdin);
                    message[strcspn(message, "\n")] = '\0';  // 개행 문자 제거

                    if (strcmp(message, "q") == 0) {
                        // Send the "disconnect" message along with the nickname to the server
                        char disconnect_message[MAX_MESSAGE_LEN + 100];
                        sprintf(disconnect_message, "%s님이 채팅방에 나가셨습니다.", nickname);
                        if (send(sock, disconnect_message, strlen(disconnect_message), 0) < 0) {
                            printf("메시지 전송 실패\n");
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
                        printf("채팅 기록을 저장하는 동안 오류가 발생했습니다.\n");
                    }

                    // Send the message to the server
                    char formatted_message[MAX_MESSAGE_LEN + 100];
                    sprintf(formatted_message, "%s: %s", nickname, message);
                    fprintf(file, "[%s] %s\n", formatted_message);
                    fclose(file);
                    if (send(sock, formatted_message, strlen(formatted_message), 0) < 0) {
                        printf("메시지 전송 실패\n");
                        return 1;
                    }
                }

                // Wait for the receive thread to exit
                WaitForSingleObject(receiveThread, INFINITE);
                CloseHandle(receiveThread);
            }
            else {
                printf("닉네임을 먼저 입력하세요.\n");
            }
            break;

        case 3:
            // 채팅 이력 텍스트 파일 탐색기를 엽니다.
            if (strlen(nickname) > 0) {
                // 채팅 이력 텍스트 파일 경로를 설정하세요.
                const char* filePath = "chat_history.txt";

                // 파일 탐색기를 엽니다.
                int result = openFileExplorer(filePath);
                if (result != 0) {
                    printf("파일 탐색기를 열 수 없습니다. 오류 코드: %d\n", result);
                }
            }
            else {
                printf("닉네임을 먼저 입력하세요.\n");
            }
            break;

        case 4:
            // 프로그램을 종료합니다.
            printf("프로그램을 종료합니다.\n");
            return 0;

        default:
            // 잘못된 선택일 경우 메인 화면으로 돌아갑니다.
            printf("잘못된 선택입니다. 메인 메뉴로 돌아갑니다.\n");
            continue;
        }
    }

    return 0;
}
