#include <zmq.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h> // For sleep

int main(void) {
    void *context = zmq_ctx_new();
    printf("Connecting to replier server...\n");
    void *requester = zmq_socket(context, ZMQ_REQ); // REQ socket for requesting
    int rc = zmq_connect(requester, "tcp://localhost:5555"); // Connects to the server
    if (rc != 0) {
        perror("Error connecting socket");
        return 1;
    }

    for (int request_nbr = 0; request_nbr < 5; request_nbr++) {
        char *request_message = "Hello";
        printf("Sending request %d...\n", request_nbr + 1);
        zmq_send(requester, request_message, strlen(request_message), 0); // Send a request

        // Get the reply.
        char buffer[10]; // Small buffer for "World"
        zmq_recv(requester, buffer, sizeof(buffer) - 1, 0);
        buffer[sizeof(buffer) - 1] = '\0'; // Null-terminate the received string
        printf("Received reply %d: %s\n", request_nbr + 1, buffer);
    }

    // Clean up
    zmq_close(requester);
    zmq_ctx_destroy(context);
    return 0;
}
