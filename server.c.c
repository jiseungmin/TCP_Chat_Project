#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <winsock2.h>

#pragma comment(lib, "ws2_32.lib") // 특정 라이브러리 파일을 자동으로 링크하도록 컴파일러에 지시하는 역할을 합니다.

#define MAX_CLIENTS 10
#define MAX_BUFFER_SIZE 1024

void HandleClient(SOCKET clientSocket);

int main() {
    WSADATA wsaData; // window 소켓 구현에 대한 정보 저장
    SOCKET serverSocket, clientSocket; // 서버소켓, 클라이언트 소켓
    SOCKADDR_IN serverAddr, clientAddr; // 서버와 클라이언트 소켓주소
    int addrLen = sizeof(clientAddr); //주소 크기

    // Initialize Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) { //  winsock 라이브러리 초기화
        printf("연결에 실패 하셨습니다.(WSAStartup)\n");
        return 1;
    }

    // Create a socket
    serverSocket = socket(AF_INET, SOCK_STREAM, 0); //AF_INET :IPv4 주소체계, SOCK_STREAM TCP프로토콜 사용, 0 프로토콜 자동 선택
    if (serverSocket == INVALID_SOCKET) { //INVALID_SOCKET 유효하지 않은 소켓을 나타내는 매크로 상수
        printf("연결에 실패 하셨습니다.(serverSocket)\n");
        return 1;
    }

    // Set server address information
    serverAddr.sin_family = AF_INET; // IPv4 주소 체계를 사용하겠다.
    serverAddr.sin_port = htons(9999); // 9999포트로 변환하여 설정
    serverAddr.sin_addr.s_addr = INADDR_ANY; // 모든 네트워크 인터페이스에서 들어오는 연결을 수신 

    // Bind the socket to the server address
    //서버 소켓을 특정 IP 주소와 포트번호에 연결하여 해당 주소와 포트에서 클라이언트이 연결을 수신할수 있도록 하는것
    if (bind(serverSocket, (SOCKADDR*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) // 서버소켓과 서버 주소 바인딩
    {
        printf("Bind 실패.\n");
        closesocket(serverSocket); // 서버 소켓 닫기
        return 1;
    }

    // Listen for incoming connections
    // listen 함수는 서버 소켓을 통해 클라이언트이 연결 요청을 수신하기 위해 소켓을 수신 대기 상태로 설정하는 역할
    if (listen(serverSocket, SOMAXCONN) == SOCKET_ERROR) { // 서버 소켓은 클라이언트의 연결 요청을 받을 수 있는 상태가 됨
        printf("LISTEN 실패.\n");
        closesocket(serverSocket);// 서버 소켓 닫기
        return 1;
    }

    printf("서버 작동중입니다. 클라이언트의 연결을 기다리는 중입니다..\n");

    while (1) {
        // Accept a client connection
        clientSocket = accept(serverSocket, (SOCKADDR*)&clientAddr, &addrLen); //클라이언트의 연결 요청을 기다리고, 연결 요청이 수락되면 새로운 소켓인 clientSocket을 생성
        if (clientSocket == INVALID_SOCKET) {
            printf("클라이언트와 통신 실패.\n");
            continue;
        }

        printf("Client connected: %s:%d\n", inet_ntoa(clientAddr.sin_addr), ntohs(clientAddr.sin_port)); 

        // Handle the client connection
        HandleClient(clientSocket); // 클라이언트에서 오는 데이터 출력 함수

        printf("Client disconnected: %s:%d\n", inet_ntoa(clientAddr.sin_addr), ntohs(clientAddr.sin_port));

        // Close the client socket
        closesocket(clientSocket); // 클라이언트 소켓 닫기
    }

    // Cleanup
    closesocket(serverSocket); // 서버 소켓 닫기
    WSACleanup(); // 라이브러리 사용한 자원 정리

    return 0;
}

void HandleClient(SOCKET clientSocket) {
    char buffer[MAX_BUFFER_SIZE];
    int bytesRead;

    while (1) {
        // Receive data from client
        bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0); // 클라이언트로부터 데이터를 받는다
        if (bytesRead <= 0) {
            break;
        }

        // Null-terminate the received data
        buffer[bytesRead] = '\0'; // 수신된 데이터 널처리

        printf("클라이언트 채팅: %s\n", buffer); // 수신된 데이터 출력

        // Echo the received data back to the client
        send(clientSocket, buffer, bytesRead, 0); // 클라이언트에게 데이터를 다시 던져줌
    }
}
