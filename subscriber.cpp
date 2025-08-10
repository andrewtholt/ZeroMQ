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
    char* url = NULL;
    char* topic = NULL;
    int opt;

    fprintf(stderr, "[SUBSCRIBER] Starting up...\n");

    while ((opt = getopt(argc, argv, "u:t:h")) != -1) {
        switch (opt) {
            case 'h':
                fprintf(stderr, "Usage: %s -u url -t topic\n\n", argv[0]);
                fprintf(stderr, "e.g. ./subscriber -u \"tcp://127.0.0.1:5556\" -t status\n");
               exit(EXIT_SUCCESS);
            case 'u':
                url = optarg;
                break;
            case 't':
                topic = optarg;
                break;
            default:
                fprintf(stderr, "Usage: %s -u url -t topic\n", argv[0]);
                exit(EXIT_FAILURE);
        }
    }

    if (url == NULL || topic == NULL) {
        fprintf(stderr, "[SUBSCRIBER] Error: The -u and -t options are required.\n");
        exit(EXIT_FAILURE);
    }

    //  Prepare our context and subscriber
    void *context = zmq_ctx_new ();
    if (!context) {
        fprintf(stderr, "[SUBSCRIBER] Error creating ZMQ context: %s\n", zmq_strerror(zmq_errno()));
        return 1;
    }
    fprintf(stderr, "[SUBSCRIBER] ZMQ context created.\n");

    void *subscriber = zmq_socket (context, ZMQ_SUB);
    if (!subscriber) {
        fprintf(stderr, "[SUBSCRIBER] Error creating ZMQ socket: %s\n", zmq_strerror(zmq_errno()));
        zmq_ctx_destroy(context);
        return 1;
    }
    fprintf(stderr, "[SUBSCRIBER] ZMQ socket created.\n");

    int rc_connect = zmq_connect (subscriber, url);
    if (rc_connect != 0) {
        fprintf(stderr, "[SUBSCRIBER] Error connecting to %s: %s\n", url, zmq_strerror(zmq_errno()));
        zmq_close(subscriber);
        zmq_ctx_destroy(context);
        return 1;
    }
    fprintf(stderr, "[SUBSCRIBER] Connected to %s.\n", url);

    // Set a receive timeout to avoid blocking indefinitely
    int timeout_ms = 2000; // 5 seconds
    int rc_timeout = zmq_setsockopt(subscriber, ZMQ_RCVTIMEO, &timeout_ms, sizeof(timeout_ms));
    if (rc_timeout != 0) {
        fprintf(stderr, "[SUBSCRIBER] Error setting RCVTIMEO: %s\n", zmq_strerror(zmq_errno()));
        zmq_close(subscriber);
        zmq_ctx_destroy(context);
        return 1;
    }
    fprintf(stderr, "[SUBSCRIBER] RCVTIMEO set to %dms.\n", timeout_ms);

    int rc_subscribe = zmq_setsockopt (subscriber, ZMQ_SUBSCRIBE, topic, strlen(topic));
    if (rc_subscribe != 0) {
        fprintf(stderr, "[SUBSCRIBER] Error subscribing to topic \"%s\": %s\n", topic, zmq_strerror(zmq_errno()));
        zmq_close(subscriber);
        zmq_ctx_destroy(context);
        return 1;
    }
    fprintf(stderr, "[SUBSCRIBER] Subscribed to topic \"%s\" at %s.\n", topic, url);

    while(1) {
        char topic_buf[256];
        char msg_buf[256];

        fprintf(stderr, "[SUBSCRIBER] Waiting for message (topic part)...\n");
        // Receive the topic part
        int topic_size = zmq_recv(subscriber, topic_buf, sizeof(topic_buf) - 1, 0);
        if (topic_size == -1) {
            if (zmq_errno() == EAGAIN) {
                fprintf(stderr, "[SUBSCRIBER] Timeout: No message received within %dms. Retrying...\n", timeout_ms);
                continue;
            } else {
                fprintf(stderr, "[SUBSCRIBER] Error receiving topic: %s\n", zmq_strerror(zmq_errno()));
                break;
            }
        }
        topic_buf[topic_size] = '\0';
        fprintf(stderr, "[SUBSCRIBER] Received topic part: '%s' (size: %d)\n", topic_buf, topic_size);

        // Check if there are more parts to the message
        int more;
        size_t more_size = sizeof(more);
        int rc_getsockopt = zmq_getsockopt(subscriber, ZMQ_RCVMORE, &more, &more_size);
        if (rc_getsockopt != 0) {
            fprintf(stderr, "[SUBSCRIBER] Error getting ZMQ_RCVMORE: %s\n", zmq_strerror(zmq_errno()));
            break;
        }

        if (!more) {
            fprintf(stderr, "[SUBSCRIBER] Received single-part message, expected multi-part. Topic: %s\n", topic_buf);
            continue;
        }
        fprintf(stderr, "[SUBSCRIBER] More message parts expected.\n");

        fprintf(stderr, "[SUBSCRIBER] Waiting for message (data part)...\n");
        // Receive the message part
        int msg_size = zmq_recv(subscriber, msg_buf, sizeof(msg_buf) - 1, 0);
        if (msg_size == -1) {
            fprintf(stderr, "[SUBSCRIBER] Error receiving message data: %s\n", zmq_strerror(zmq_errno()));
            break;
        }
        msg_buf[msg_size] = '\0';
        fprintf(stderr, "[SUBSCRIBER] Received data part: '%s' (size: %d)\n", msg_buf, msg_size);

        printf("[%s] %s\n", topic_buf, msg_buf);
        break; // Exit after receiving one message
    }

    fprintf(stderr, "[SUBSCRIBER] Closing socket and context.\n");
    zmq_close (subscriber);
    zmq_ctx_destroy (context);
    fprintf(stderr, "[SUBSCRIBER] Exiting.\n");
    return 0;
}
