//  Subscriber for ZeroMQ
//  Connects to a URL and subscribes to a topic, then prints received messages.

#include <zmq.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h> // For errno and EWOULDBLOCK
#include <string> // For std::string

int main (int argc, char *argv[])
{
    char* url = (char*)"tcp://127.0.0.1:5558"; // Hardcoded URL from zmqConfig.json
    char* topic = (char*)"OUTPUT";             // Hardcoded topic

    //  Prepare our context and subscriber
    void *context = zmq_ctx_new ();
    if (!context) {
        fprintf(stderr, "Error creating ZMQ context: %s\n", zmq_strerror(zmq_errno()));
        return 1;
    }

    void *subscriber = zmq_socket (context, ZMQ_SUB);
    if (!subscriber) {
        fprintf(stderr, "Error creating ZMQ socket: %s\n", zmq_strerror(zmq_errno()));
        zmq_ctx_destroy(context);
        return 1;
    }

    int rc_connect = zmq_connect (subscriber, url);
    if (rc_connect != 0) {
        fprintf(stderr, "Error connecting to %s: %s\n", url, zmq_strerror(zmq_errno()));
        zmq_close(subscriber);
        zmq_ctx_destroy(context);
        return 1;
    }

    int rc_subscribe = zmq_setsockopt (subscriber, ZMQ_SUBSCRIBE, topic, strlen(topic));
    if (rc_subscribe != 0) {
        fprintf(stderr, "Error subscribing to topic \"%s\": %s\n", topic, zmq_strerror(zmq_errno()));
        zmq_close(subscriber);
        zmq_ctx_destroy(context);
        return 1;
    }
    printf("Subscribed to topic \"%s\" at %s. Waiting for messages...\n", topic, url);

    while(1) {
        char topic_buf[256];
        char msg_buf[256];

        // Receive the topic part
        int topic_size = zmq_recv(subscriber, topic_buf, sizeof(topic_buf) - 1, 0);
        if (topic_size == -1) {
            // Handle error or timeout if necessary, but for continuous display, just continue
            continue;
        }
        topic_buf[topic_size] = '\0';

        // Check if there are more parts to the message
        int more;
        size_t more_size = sizeof(more);
        zmq_getsockopt(subscriber, ZMQ_RCVMORE, &more, &more_size);

        if (!more) {
            // This case should ideally not happen for PUB-SUB with topic + message
            printf("Received single-part message. Topic: %s\n", topic_buf);
            continue;
        }

        // Receive the message part
        int msg_size = zmq_recv(subscriber, msg_buf, sizeof(msg_buf) - 1, 0);
        if (msg_size == -1) {
            // Handle error or timeout if necessary
            continue;
        }
        msg_buf[msg_size] = '\0';

        printf("[%s] %s\n", topic_buf, msg_buf);
    }

    // This part will not be reached in the infinite loop, but good practice for cleanup
    zmq_close (subscriber);
    zmq_ctx_destroy (context);
    return 0;
}