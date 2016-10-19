#include <nanopipe/nanopipe.hpp>

int main(int argc, char **argv)
{
    nanopipe_init();

    // Instantiate the source and the send
    AMQPSource amqp_source("localhost", 5672, "myqueue");
    MQTTSend mqtt_send("broker.hivemq.com", 1883, "mytopic");
    
    // Connect the send into the source
    amqp_source.addSend(&mqtt_send);
    
    // Instantiate the manager
    NanoManager manager;
    manager.addSource(&amqp_source);
    manager.addSend(&mqtt_send);

    // Wait forever
    manager.startAll();
    manager.waitAll();

    return 0;
}
