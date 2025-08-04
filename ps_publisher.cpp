//  Weather update server
//  Binds PUB socket to tcp://*:5556
//  Publishes random weather updates

#include <zmq.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <unistd.h>

#define within(num) (int) ((float) num * random () / (RAND_MAX + 1.0))

int main (void)
{
    //  Prepare our context and publisher
    void *context = zmq_ctx_new ();
    void *publisher = zmq_socket (context, ZMQ_PUB);
    zmq_bind (publisher, "tcp://*:5556");
    sleep(1); //  Give subscriber time to connect

    //  Initialize random number generator
    srandom ((unsigned) time (NULL));
    while (1) {
        //  Get values that will fool the boss
        int zipcode, temperature, relhumidity;
        zipcode     = within (100000);
        temperature = within (215) - 80;
        relhumidity = within (50) + 10;

        //  Send message to all subscribers
        char update [40];
        sprintf (update, "%05d %d %d", zipcode, temperature, relhumidity);
        zmq_send (publisher, update, strlen(update), 0);
        sleep(1);
    }
    zmq_close (publisher);
    zmq_ctx_destroy (context);
    return 0;
}
