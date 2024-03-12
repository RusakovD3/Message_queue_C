#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mqueue.h>

#define QUEUE_NAME "/my_queue"
#define MAX_SIZE 1024
#define MSG_HI "Hi"

int main() {
    mqd_t mq;
    char buffer[MAX_SIZE + 1];

    mq = mq_open(QUEUE_NAME, O_RDWR);
    if (mq == -1) {
        perror("mq_open");
        exit(1);
    }

    printf("Client: Waiting for message...\n");
    ssize_t bytes_read = mq_receive(mq, buffer, MAX_SIZE, NULL);
    if (bytes_read >= 0) {
        buffer[bytes_read] = '\0';
        printf("Client: Received '%s'\n", buffer);
    } else {
        perror("mq_receive");
        exit(2);
    }

    printf("Client: Sending 'Hi'\n");
    if (mq_send(mq, MSG_HI, strlen(MSG_HI), 0) == -1) {
        perror("mq_send");
        exit(3);
    }

    mq_close(mq);
    printf("Client: Done.\n");

    return 0;
}
