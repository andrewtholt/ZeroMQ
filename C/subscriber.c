#include <zmq.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

int main(void) {
    void *context = zmq_ctx_new();
    void *subscriber = zmq_socket(context, ZMQ_SUB);

    int rc = zmq_connect(subscriber, "tcp://127.0.0.1:5558");
    if (rc != 0) {
        perror("Error connecting socket");
        return 1;
    }

    const char *topic = "OUTPUTS";
    rc = zmq_setsockopt(subscriber, ZMQ_SUBSCRIBE, topic, strlen(topic));
    if (rc != 0) {
        perror("Error setting socket option");
        return 1;
    }

    printf("Subscribed to topic: %s\n", topic);

    while (1) {
        char topic_buf[256];
        char msg_buf[256];

        int topic_size = zmq_recv(subscriber, topic_buf, 255, 0);
        if (topic_size == -1) {
            perror("Error receiving topic");
            continue;
        }
        topic_buf[topic_size] = '\0';

        int msg_size = zmq_recv(subscriber, msg_buf, 255, 0);
        if (msg_size == -1) {
            perror("Error receiving message");
            continue;
        }
        msg_buf[msg_size] = '\0';

        printf("Received message on topic '%s': %s\n", topic_buf, msg_buf);
    }

    zmq_close(subscriber);
    zmq_ctx_destroy(context);

    return 0;
}
