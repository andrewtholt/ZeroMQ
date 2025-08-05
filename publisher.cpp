//  Publisher for ZeroMQ
//  Takes a URL and a message and publishes the message to the URL.

#include <zmq.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main (int argc, char *argv[])
{
    char* url = NULL;
    char* message = NULL;
    int opt;

    while ((opt = getopt(argc, argv, "u:m:h")) != -1) {
        switch (opt) {
            case 'h':
                fprintf(stderr, "Usage: %s -u url -m message\n", argv[0]);
                exit(EXIT_SUCCESS);
            case 'u':
                url = optarg;
                break;
            case 'm':
                message = optarg;
                break;
            default:
                fprintf(stderr, "Usage: %s -u url -m message\n", argv[0]);
                exit(EXIT_FAILURE);
        }
    }

    if (url == NULL || message == NULL) {
        fprintf(stderr, "Both -u and -m options are required.\n");
        exit(EXIT_FAILURE);
    }

    //  Prepare our context and publisher
    void *context = zmq_ctx_new ();
    void *publisher = zmq_socket (context, ZMQ_PUB);
    zmq_bind (publisher, url);
    sleep(1); //  Give subscriber time to connect

    //  Send message to all subscribers
    zmq_send (publisher, message, strlen(message), 0);
    printf("Published message \"%s\" to %s\n", message, url);

    zmq_close (publisher);
    zmq_ctx_destroy (context);
    return 0;
}
