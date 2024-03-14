#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <mqueue.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <signal.h>

#define SERVER_QUEUE_NAME "/server_queue"
#define CLIENT_QUEUE_NAME "/client_queue"
#define QUEUE_PERMISSIONS 0660
#define MAX_MESSAGES 10
#define MAX_MSG_SIZE 256

int main() {
    mqd_t qd_server, qd_client;
    char in_buffer[MAX_MSG_SIZE];
    char out_buffer[MAX_MSG_SIZE];

    struct mq_attr attr;
    attr.mq_flags = 0;
    attr.mq_maxmsg = MAX_MESSAGES;
    attr.mq_msgsize = MAX_MSG_SIZE;
    attr.mq_curmsgs = 0;

    // Инициализация очередей
    qd_server = mq_open(SERVER_QUEUE_NAME, O_RDWR | O_CREAT, QUEUE_PERMISSIONS, &attr);
    if (qd_server == -1) {
        perror("Server queue mq_open");
        exit(1);
    }

    qd_client = mq_open(CLIENT_QUEUE_NAME, O_RDWR | O_CREAT, QUEUE_PERMISSIONS, &attr);
    if (qd_client == -1) {
        perror("Client queue mq_open");
        exit(1);
    }

    // Инициализация ncurses
    initscr();
    cbreak();
    noecho();
    curs_set(1);
    int rows, cols;
    getmaxyx(stdscr, rows, cols); // Получение размеров терминала
    WINDOW *input_win = newwin(3, cols, rows - 3, 0); // Окно для ввода
    WINDOW *output_win = newwin(rows - 3, cols, 0, 0); // Окно для вывода
    keypad(input_win, TRUE);
    scrollok(output_win, TRUE);
    wrefresh(output_win);
    wrefresh(input_win);
    box(input_win, 0, 0); 

    pid_t child_pid;
    child_pid = fork();

    if(child_pid < 0){
        perror("Error in creating process");
        exit(1);
    }
    if(child_pid == 0){
        while (1) {
            ssize_t bytes_read;
            bytes_read = mq_receive(qd_client, in_buffer, MAX_MSG_SIZE, NULL);
            if (bytes_read >= 0) {
                in_buffer[bytes_read] = '\0';
                wprintw(output_win, "Received: %s\n", in_buffer);
                wrefresh(output_win);

            } else {
                wprintw(output_win, "ERROR: mq_receive\n");
                wrefresh(output_win);
            }
        }
        exit(0);
    } else {
        echo();
        while (1) {
            werase(input_win);
            wmove(input_win, 1, 1);
            box(input_win, 0, 0); // Восстановление рамки окна ввода
            mvwprintw(input_win, 1, 1, "Enter message: ");

            wgetnstr(input_win, out_buffer, MAX_MSG_SIZE); // Чтение ввода пользователя
            wrefresh(input_win); // Обновление окна ввода

            if (strcmp(out_buffer, "exit") == 0) {
                break; // Выход из цикла, если пользователь ввел "exit"
            }

            mq_send(qd_server, out_buffer, strlen(out_buffer) + 1, 0);
        }

        // Принудительное завершение дочернего процесса
        if (kill(child_pid, SIGKILL) == -1) {
            exit(EXIT_FAILURE);
        }
    }    

    // Освобождение ресурсов ncurses
    delwin(input_win);
    delwin(output_win);
    endwin();

    // Закрытие очередей и завершение работы
    mq_close(qd_server);
    mq_close(qd_client);
    mq_unlink(SERVER_QUEUE_NAME);
    mq_unlink(CLIENT_QUEUE_NAME);

    return 0;
}
