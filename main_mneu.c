#include <stdio.h>
#include <string.h>

int main() {
  // �г����� �Է��մϴ�.
  char nickname[100];
  printf("Enter your nickname: ");
  fgets(nickname, sizeof(nickname), stdin);
  strtok(nickname, "\n");  // ���� ���� ����

  // ���� �޴��� ǥ���մϴ�.
  while (1) {
    printf("1. Enter a message\n");
    printf("2. Change your nickname\n");
    printf("3. Quit\n");
    int choice;
    scanf("%d", &choice);
    char message[1000];

    switch (choice) {
      case 1:
        // �޽����� �Է��մϴ�.
        
        printf("Enter your message: ");
        getchar();  // �Է� ���� ����
        fgets(message, sizeof(message), stdin);
        strtok(message, "\n");  // ���� ���� ����

        // �޽����� ó���ϴ� �ڵ带 �ۼ��ϼ���.
        printf("Message: %s\n", message);
        break;

      case 2:
        // �� �г����� �Է��մϴ�.
        printf("Enter your new nickname: ");
        getchar();  // �Է� ���� ����
        fgets(nickname, sizeof(nickname), stdin);
        strtok(nickname, "\n");  // ���� ���� ����

        // �г����� ó���ϴ� �ڵ带 �ۼ��ϼ���.
        printf("Nickname: %s\n", nickname);
        break;

      case 3:
        // ���α׷��� �����մϴ�.
        return 0;

      default:
        // �߸��� ������ ��� ���� ȭ������ ���ư��ϴ�.
        printf("Invalid choice. Returning to the main menu.\n");
        continue;
    }
  }
}
