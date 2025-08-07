//  Weather update client
//  Connects SUB socket to tcp://localhost:5556
//  Collects weather updates and finds avg temp in zipcode
//  Also publishes the INPUT section of zmqConfig.json

#include <zmq.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <fstream>
#include <string>
#include "nlohmann/json.hpp"

// for convenience
using json = nlohmann::json;

int main (int argc, char *argv [])
{
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

    //  Socket to talk to server (subscriber)
    printf ("Collecting status...\n");
    void *subscriber = zmq_socket (context, ZMQ_SUB);
    zmq_connect (subscriber, "tcp://127.0.0.1:5556");

    //  Subscribe to all zipcodes by default
    const char *filter = (argc > 1)? argv [1]: "status";
    zmq_setsockopt (subscriber, ZMQ_SUBSCRIBE,
                         filter, strlen (filter));
    printf("Ready to receive messages...\n");

    // Socket to publish responses (publisher)
    void *publisher = zmq_socket(context, ZMQ_PUB);
    std::string pub_url = config["INPUT"]["URL"];
    // ZMQ uses tcp, not http
    pub_url.replace(0, 4, "tcp");
    zmq_bind(publisher, pub_url.c_str());
    printf("Publishing INPUT data to %s\n", pub_url.c_str());


    //  Process 10 updates
//    for (int update_nbr = 0; update_nbr < 10; update_nbr++) {
        char string [256];
        zmq_recv (subscriber, string, 255, 0);

        printf("Received: %s\n",string);

        // Now, publish the INPUT section of the config
        json input_data = config["INPUT"];
        std::string message = input_data.dump();
        const char* topic = "INPUTS";

        zmq_send(publisher, topic, strlen(topic), ZMQ_SNDMORE);
        zmq_send(publisher, message.c_str(), message.length(), 0);
        printf("Published INPUT data.\n");
//    }

    zmq_close (subscriber);
    zmq_close (publisher);
    zmq_ctx_destroy (context);
    return 0;
}
