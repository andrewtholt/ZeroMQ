#include <zmq.h>
#include <stdio.h>
#include <unistd.h> // For sleep
#include <string.h>

int main(void) {
    void *context = zmq_ctx_new();
    void *responder = zmq_socket(context, ZMQ_REP); // REP socket for replying
    int rc = zmq_bind(responder, "tcp://*:5555");  // Binds to all available interfaces on port 5555
    if (rc != 0) {
        perror("Error binding socket");
        return 1;
    }

    printf("Replier server started. Waiting for requests...\n");

    while (1) {
        // Wait for next request from client
        char buffer[10]; // Small buffer for "Hello"
        zmq_recv(responder, buffer, sizeof(buffer) - 1, 0);
        buffer[sizeof(buffer) - 1] = '\0'; // Null-terminate the received string
        printf("Received request: %s\n", buffer);

        // Do some 'work'
        sleep(1);

        // Send reply back to client
        char *reply_message = "World";
        zmq_send(responder, reply_message, strlen(reply_message), 0);
        printf("Sent reply: %s\n", reply_message);
    }

    // Clean up (this part might not be reached in this infinite loop)
    zmq_close(responder);
    zmq_ctx_destroy(context);
    return 0;
}
