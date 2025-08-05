#include <zmq.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h> // For sleep
#include <chrono> // For timing
#include <iostream> // For std::cout
#include <vector>
#include <map>
#include "nlohmann/json.hpp"

using namespace std;
map<string,string> statusRecord;

void init() {
    statusRecord["INPUT"]  = "\{ \"STATUS\" : \"DOWN\" }";
    statusRecord["OUTPUT"] = "\{ \"STATUS\" : \"DOWN\" }";
}

int main(void) {
    void *context = zmq_ctx_new();
    printf("Connecting to replier server...\n");
    void *requester = zmq_socket(context, ZMQ_REQ); // REQ socket for requesting
    int timeout_ms = 2000; // 2 seconds
    zmq_setsockopt(requester, ZMQ_RCVTIMEO, &timeout_ms, sizeof(timeout_ms));
    int rc = zmq_connect(requester, "tcp://localhost:5555"); // Connects to the server
    if (rc != 0) {
        perror("Error connecting socket");
        return 1;
    }

    for (int request_nbr = 0; request_nbr < 5; request_nbr++) {
        char request_message[10];
        memset(request_message,0,6);
        strcpy(request_message,"STATUS");
        printf(">%s< Sending request %d...\n", request_message, request_nbr + 1);
        auto start_time = std::chrono::high_resolution_clock::now();
//        zmq_send(requester, request_message, strlen(request_message), 0); // Send a request
        printf("len=%ld\n", strlen(request_message));
        zmq_send(requester, request_message, strlen(request_message), 0); // Send a request

        // Get the reply.
        char buffer[10]; // Small buffer for "World"
        int bytes_received = zmq_recv(requester, buffer, sizeof(buffer) - 1, 0);
        auto end_time = std::chrono::high_resolution_clock::now();

        if (bytes_received == -1) {
            if (zmq_errno() == ETIMEDOUT) {
                std::cout << "Request timed out! No reply received within 2 seconds.\n";
            } else {
                perror("Error receiving message");
            }
        } else {
            buffer[bytes_received] = '\0'; // Null-terminate the received string
            printf("Received reply %d: %s\n", request_nbr + 1, buffer);

            std::chrono::duration<double> duration = end_time - start_time;
            bool slow_reply = false;
            if (duration.count() > 1.0) {
                slow_reply = true;
                std::cout << "Reply took longer than 1 second! Duration: " << duration.count() << "s\n";
            }
        }
    }

    // Clean up
    zmq_close(requester);
    zmq_ctx_destroy(context);
    return 0;
}
