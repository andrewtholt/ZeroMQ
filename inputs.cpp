//  Weather update client
//  Connects SUB socket to tcp://localhost:5556
//  Collects weather updates and finds avg temp in zipcode

#include <zmq.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <fstream>
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


    //  Socket to talk to server
    printf ("Collecting status...\n");
    void *context = zmq_ctx_new ();
    void *subscriber = zmq_socket (context, ZMQ_SUB);
    zmq_connect (subscriber, "tcp://127.0.0.1:5556");

    //  Subscribe to all zipcodes by default
//    const char *filter = (argc > 1)? argv [1]: "";
    const char *filter = (argc > 1)? argv [1]: "status";
    zmq_setsockopt (subscriber, ZMQ_SUBSCRIBE,
                         filter, strlen (filter));
    printf("Ready to receive messages...\n");

    //  Process 10 updates
    int update_nbr;
    long total_temp = 0;
    for (update_nbr = 0; update_nbr < 10; update_nbr++) {
        char string [256];
        zmq_recv (subscriber, string, 255, 0);

        printf("%s\n",string);
        int zipcode, temperature, relhumidity;
        sscanf (string, "%d %d %d",
            &zipcode, &temperature, &relhumidity);
        total_temp += temperature;

    }
    printf ("Average temperature for zipcode '%s' was %dF\n",
        filter, (int) (total_temp / update_nbr));

    zmq_close (subscriber);
    zmq_ctx_destroy (context);
    return 0;
}
