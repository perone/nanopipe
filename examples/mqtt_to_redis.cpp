#include <nanopipe/nanopipe.hpp>

int main(int argc, char **argv)
{
    nanopipe_init();

    // Instantiate the source and the send
    MQTTSource mqtt_source("broker.hivemq.com", 1883, "wstopic");
    RedisSend redis_send("localhost", 6379, "wstopic");
    
    // Connect the send into the source
    mqtt_source.addSend(&redis_send);
    
    // Instantiate the manager
    NanoManager manager;
    manager.addSource(&mqtt_source);
    manager.addSend(&redis_send);

    // Wait forever
    manager.startAll();
    manager.waitAll();

    return 0;
}
