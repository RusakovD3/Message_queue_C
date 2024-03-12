#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/msg.h>
#include <unistd.h>
#include "message.h"

int main() {
    char cwd[1024];
    char file_path[2048];
    char *file_name = "my_msg_sys_que";
    size_t needed;
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        // Строим путь к файлу
        needed = snprintf(NULL, 0, "%s/%s", cwd, file_name) + 1;
        if (needed > sizeof(file_path)) {
            fprintf(stderr, "Ошибка: не хватает размера буфера для пути и имени файла.\n");
            return 1;
        }
        snprintf(file_path, sizeof(file_path), "%s/%s", cwd, file_name);
    } else {
        perror("getcwd() error");
        return 1;
    }

    key_t key = ftok(file_path, 'b');
    int msgid = msgget(key, 0666 | IPC_CREAT);
    if (msgid == -1) {
        fprintf(stderr, "msgget failed\n");
        exit(EXIT_FAILURE);
    }

    struct my_msg_st some_data;
    long int msg_to_receive = 2;

    // Читаем сообщение
    if (msgrcv(msgid, (void *)&some_data, MAX_TEXT, msg_to_receive, 0) == -1) {
        fprintf(stderr, "msgrcv failed\n");
        exit(EXIT_FAILURE);
    }
    printf("Received: %s\n", some_data.some_text);

    // Отправляем ответ
    some_data.my_msg_type = 1;
    strcpy(some_data.some_text, "Hi.");
    if (msgsnd(msgid, (void *)&some_data, MAX_TEXT, 0) == -1) {
        fprintf(stderr, "msgsnd failed\n");
        exit(EXIT_FAILURE);
    }

    exit(EXIT_SUCCESS);
}
