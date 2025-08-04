#include <zmq.h>
#include <stdio.h>
#include <unistd.h> // For sleep
// #include <string.h>
#include <string>
#include <cstring>
#include <ostream>
#include <iostream>

using namespace std;

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
        memset(buffer,0,sizeof(buffer));

        string reply="";

        zmq_recv(responder, buffer, sizeof(buffer) - 1, 0);
        buffer[sizeof(buffer) - 1] = '\0'; // Null-terminate the received string
        
        string cmd = buffer;
        printf("Received request: >%s<\n", buffer);
        if ( cmd == "STATUS") {
            cout << "The command is " + cmd << endl;
            reply = "{ \"STATUS\" : \"OK\" } ";
        }

        // Do some 'work'
        sleep(1);

        // Send reply back to client
//        const char *reply_message = "{ }";
        zmq_send(responder, reply.c_str(), reply.length(), 0);
        printf("Sent reply: %s\n", reply.c_str());
    }

    // Clean up (this part might not be reached in this infinite loop)
    zmq_close(responder);
    zmq_ctx_destroy(context);
    return 0;
}
