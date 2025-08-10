//  Weather update client
//  Connects SUB socket to tcp://localhost:5556
//  Also publishes the INPUT section of zmqConfig.json

#include <zmq.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <fstream>
#include <string>
#include <unistd.h> // For sleep() and pause()
#include <signal.h> // For signal handling
#include "nlohmann/json.hpp"

// for convenience
using namespace std;
using json = nlohmann::json;

// Global flag to indicate if SIGUSR1 has been received
volatile sig_atomic_t publish_flag = 0;

// Signal handler for SIGUSR1
void sigusr1_handler(int signum) {
    publish_flag = 1;
}

int main (int argc, char *argv [])
{
    // Register the signal handler for SIGUSR1
    if (signal(SIGUSR1, sigusr1_handler) == SIG_ERR) {
        perror("signal");
        return 1;
    }

    // Load and parse the configuration file
    std::ifstream configFile("zmqConfig.json");
    if (!configFile.is_open()) {
        std::cerr << "Error: Could not open zmqConfig.json" << std::endl;
        return 1;
    }

    json config;
    try
    {
        configFile >> config;
    }
    catch (json::parse_error& e)
    {
        std::cerr << "Error: Failed to parse zmqConfig.json: " << e.what() << std::endl;
        return 1;
    }

    std::cout << "Successfully loaded and parsed zmqConfig.json:" << std::endl;
    std::cout << config.dump(4) << std::endl;
    std::cout << "------------------------------------------------" << std::endl;


    //  Prepare our context
    void *context = zmq_ctx_new ();

    // Socket to publish responses (publisher)
    void *publisher = zmq_socket(context, ZMQ_PUB);
    std::string pub_url = config["INPUT"]["URL"];
    // ZMQ uses tcp, not http
    pub_url.replace(0, 4, "tcp");
    zmq_bind(publisher, pub_url.c_str());
    printf("inputs.cpp ready. Waiting for SIGUSR1 to publish.\n");

    while(true) {
        // Wait for a signal to arrive
        pause();

        // Check if SIGUSR1 was received
        if (publish_flag) {
            // Reset the flag
            publish_flag = 0;

            // Publish the INPUT section of the config
            json input_data = config["INPUT"];
            std::string message = input_data.dump();
            const char* topic = "INPUT";

            zmq_send(publisher, topic, strlen(topic), ZMQ_SNDMORE);
            zmq_send(publisher, message.c_str(), message.length(), 0);
            cout << "Published topic " << topic << " Message " << message.c_str() << endl ;
            printf("Published INPUT data.\n");
        }
    }

    zmq_close (publisher);
    zmq_ctx_destroy (context);
    return 0;
}