//  Weather update client
//  Connects SUB socket to tcp://localhost:5556
//  Collects weather updates and finds avg temp in zipcode

#include <zmq.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <string>
#include <fstream>
#include <unistd.h> // For sleep()
#include "nlohmann/json.hpp"

// for convenience
using namespace std;
using json = nlohmann::json;

int main(int argc, char *argv[])
{

    cout << "Loading template json." << endl;
    // Load and parse the configuration file
    ifstream configFile("zmqConfig.json");

    if (!configFile.is_open())
    {
        std::cerr << "Error: Could not open zmqConfig.json" << std::endl;
        return 1;
    }

    json config;
    try
    {
        configFile >> config;
    }
    catch (json::parse_error &e)
    {
        std::cerr << "Error: Failed to parse zmqConfig.json: " << e.what() << std::endl;
        return 1;
    }

    std::cout << "Successfully loaded and parsed zmqConfig.json:" << std::endl;
    std::cout << config.dump(4) << std::endl;
    std::cout << "------------------------------------------------" << std::endl;

    //  Socket to talk to server
    printf("Collecting status...\n");
    void *context = zmq_ctx_new();
    void *subscriber = zmq_socket(context, ZMQ_SUB);
    zmq_connect(subscriber, "tcp://127.0.0.1:5557");

    //  Subscribe to the INPUTS topic
    const char *filter = "INPUT";
    zmq_setsockopt(subscriber, ZMQ_SUBSCRIBE, filter, strlen(filter));
    printf("Ready to receive messages on topic '%s'...\n", filter);

    //  Receive the topic envelope
    char topic[256];

    bool runFlag = true;
    void *publisher = nullptr;

    do
    {

        memset(topic, 0, sizeof(topic)); // Clear the buffer

        zmq_recv(subscriber, topic, 255, 0);

        // Receive the JSON data
        char buffer[256];
        int size = zmq_recv(subscriber, buffer, 255, 0);
        if (size == -1)
        {
            perror("zmq_recv");
            zmq_close(subscriber);
            zmq_ctx_destroy(context);
            return 1;
        }

        printf("Received message on topic: %s\n", topic);
//        bool logicState = false;

        // Parse the JSON string
        try
        {
            std::string json_data(buffer, size);
            json received_json = json::parse(json_data);
            std::cout << "Successfully parsed JSON:" << std::endl;
            std::cout << received_json.dump(4) << std::endl;

            // Access and print the specific value
            if (received_json.contains("START") && received_json["START"].contains("VALUE"))
            {
                bool start_value = received_json["START"]["VALUE"];
                std::cout << "\nINPUT.START.VALUE: " << (start_value ? "true" : "false") << std::endl;

                logicState = start_value;
            }
            if (received_json.contains("STOP") && received_json["STOP"].contains("VALUE"))
            {
                bool stop_value = received_json["STOP"]["VALUE"];
                std::cout << "\nINPUT.STOP.VALUE: " << (stop_value ? "true" : "false") << std::endl;

                logicState = logicState && (!stop_value);
            }

            cout << "Result is " << boolalpha << logicState << endl;

            // Publish the OUTPUT section
            publisher = zmq_socket(context, ZMQ_PUB);
            std::string pub_url = config["OUTPUT"]["URL"];
            pub_url.replace(0, 4, "tcp"); // ZMQ uses tcp, not http
            zmq_bind(publisher, pub_url.c_str());
            sleep(2); // Give subscriber time to connect
            printf("Publishing OUTPUT data to %s\n", pub_url.c_str());

            json output_data = config["OUTPUT"];
            output_data["MOTOR"]["VALUE"] = logicState;
            std::string message = output_data.dump();
            const char *out_topic = "OUTPUT";

            cout << "Publishing topic " << out_topic << " Message " << message.c_str() << endl;

            zmq_send(publisher, out_topic, strlen(out_topic), ZMQ_SNDMORE);
            zmq_send(publisher, message.c_str(), message.length(), 0);
            printf("Published OUTPUT data.\n");
        }
        catch (json::parse_error &e)
        {
            std::cerr << "Error: Failed to parse received JSON: " << e.what() << std::endl;
        }
    } while (runFlag);

    zmq_close(publisher);
    zmq_close(subscriber);
    zmq_ctx_destroy(context);
    return 0;
}
