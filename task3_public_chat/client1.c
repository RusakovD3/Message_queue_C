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
#define MAX_MSG_SIZE 256

int main() {
    mqd_t qd_server, qd_client;
    char out_buffer[MAX_MSG_SIZE];
    
    // Подключение к очередям
    qd_server = mq_open(SERVER_QUEUE_NAME, O_RDONLY);
    if (qd_server == -1) {
        perror("Server queue mq_open");
        exit(1);
    }

    qd_client = mq_open(CLIENT_QUEUE_NAME, O_RDWR);
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
    scrollok(output_win, TRUE);
    keypad(input_win, TRUE); // Включить поддержку функциональных клавиш в input_win
    box(input_win, 0, 0); // Рисование рамки вокруг окна ввода
    // Необходимо вызывать wrefresh для обновления окна после box
    wrefresh(output_win);
    wrefresh(input_win);

    pid_t child_pid;
    child_pid = fork();

    if(child_pid < 0){
        perror("Error in creating process");
        exit(1);
    }
    if(child_pid == 0){
        while (1) {
            if(mq_receive(qd_server, out_buffer, MAX_MSG_SIZE, NULL) >= 0) {
                wprintw(output_win, "Received from server: %s\n", out_buffer);
                wrefresh(output_win);
            }
            // Чтение сообщений от клиента (если предполагается)
            if(mq_receive(qd_client, out_buffer, MAX_MSG_SIZE, NULL) >= 0) {
                wprintw(output_win, "Received from client: %s\n", out_buffer);
                wrefresh(output_win);
            }
        }
        exit(0);
    } else {
        echo();
        while(1) {
            wmove(input_win, 1, 1);
            werase(input_win);
            box(input_win, 0, 0); 
            mvwprintw(input_win, 1, 1, "Enter message: ");
            wgetnstr(input_win, out_buffer, MAX_MSG_SIZE); // Чтение ввода пользователя
            wrefresh(input_win); // Обновление окна ввода

            if (strcmp(out_buffer, "exit") == 0) {
                break; // Выход из цикла, если пользователь ввел "exit"
            }

            // Отправка сообщения серверу
            mq_send(qd_client, out_buffer, strlen(out_buffer) + 1, 0);
        }

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

    return 0;
}
