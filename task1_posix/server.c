#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mqueue.h>
#include <sys/stat.h>
#include <signal.h>

#define QUEUE_NAME "/my_queue"
#define MAX_SIZE 1024
#define MSG_STOP "terminate"
#define MSG_HELLO "Hello"

int main() {
    mqd_t mq;
    struct mq_attr attr;
    char buffer[MAX_SIZE + 1];

    attr.mq_flags = 0;
    attr.mq_maxmsg = 10;
    attr.mq_msgsize = MAX_SIZE;
    attr.mq_curmsgs = 0;

    mq = mq_open(QUEUE_NAME, O_CREAT | O_RDWR, 0644, &attr);
    if (mq == -1) {
        perror("mq_open");
        exit(1);
    }

    printf("Server: Sending 'Hello'\n");
    if (mq_send(mq, MSG_HELLO, strlen(MSG_HELLO), 0) == -1) {
        perror("mq_send");
        exit(2);
    }

    printf("Server: Waiting for response...\n");

    struct sigevent notif;
    sigset_t sig_set;
    siginfo_t info; 

    // Сигналом уведомления является SIGUSR1.
    sigemptyset(&sig_set);
    sigaddset(&sig_set, SIGUSR1);

    // Блокируем SIGUSR1, так как мы будем ждать его в вызове sigwaitinfo
    sigprocmask(SIG_BLOCK, &sig_set, NULL);
    
    // Теперь настраиваем уведомление
    notif.sigev_notify = SIGEV_SIGNAL;
    notif.sigev_signo = SIGUSR1;

    if (mq_notify(mq, &notif)){
        perror("mq_notify");
        return -1;
    }
    // Если в очередь поступит сообщение, будет доставлен SIGUSR1
    do {
        sigwaitinfo(&sig_set, &info);

    } while(info.si_signo != SIGUSR1);
   

    // Теперь можно принять сообщение.
    ssize_t bytes_read = mq_receive(mq, buffer, MAX_SIZE, NULL);
    if (bytes_read >= 0) {
        buffer[bytes_read] = '\0';
        printf("Server: Received '%s'\n", buffer);
    } else {
        perror("mq_receive");
        exit(3);
    }

    mq_close(mq);
    mq_unlink(QUEUE_NAME);
    printf("Server: Done.\n");

    return 0;
}
