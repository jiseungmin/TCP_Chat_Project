#include <stdio.h>
#include <string.h>

int main() {
  // 닉네임을 입력합니다.
  char nickname[100];
  printf("Enter your nickname: ");
  fgets(nickname, sizeof(nickname), stdin);
  strtok(nickname, "\n");  // 개행 문자 제거

  // 메인 메뉴를 표시합니다.
  while (1) {
    printf("1. Enter a message\n");
    printf("2. Change your nickname\n");
    printf("3. Quit\n");
    int choice;
    scanf("%d", &choice);
    char message[1000];

    switch (choice) {
      case 1:
        // 메시지를 입력합니다.
        
        printf("Enter your message: ");
        getchar();  // 입력 버퍼 비우기
        fgets(message, sizeof(message), stdin);
        strtok(message, "\n");  // 개행 문자 제거

        // 메시지를 처리하는 코드를 작성하세요.
        printf("Message: %s\n", message);
        break;

      case 2:
        // 새 닉네임을 입력합니다.
        printf("Enter your new nickname: ");
        getchar();  // 입력 버퍼 비우기
        fgets(nickname, sizeof(nickname), stdin);
        strtok(nickname, "\n");  // 개행 문자 제거

        // 닉네임을 처리하는 코드를 작성하세요.
        printf("Nickname: %s\n", nickname);
        break;

      case 3:
        // 프로그램을 종료합니다.
        return 0;

      default:
        // 잘못된 선택일 경우 메인 화면으로 돌아갑니다.
        printf("Invalid choice. Returning to the main menu.\n");
        continue;
    }
  }
}
