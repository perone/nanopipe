#include <chrono>

#include <nanopipe/nanopipe.hpp>

int main(int argc, char **argv)
{
    nanopipe_init();

    AMQPSource amqp_source1("localhost", 5672, "my-queue");
    AMQPSend amqp_send1("localhost", 5672, "exch", "keylero");
    
    RedisSource redis_source1("localhost", 6379, "mytopic");
    MQTTSource mqtt_source1("broker.hivemq.com", 1883, "nanopipe");
    RedisSend redis_send1("localhost", 6379, "sink");
    MQTTSend mqtt_send("broker.hivemq.com", 1883, "sinkpipe");
    StreamSend stream_send1(&std::cout);
    
    redis_source1.addSend(&redis_send1);
    redis_source1.addSend(&mqtt_send);
    redis_source1.addSend(&stream_send1);
    redis_source1.addSend(&amqp_send1);
    
    mqtt_source1.addSend(&mqtt_send);
    mqtt_source1.addSend(&stream_send1);
    mqtt_source1.addSend(&redis_send1);
    
    amqp_source1.addSend(&stream_send1);

    NanoManager manager;
    manager.addSource(&amqp_source1);
    manager.addSource(&redis_source1);  
    manager.addSource(&mqtt_source1);
    manager.addSend(&stream_send1);
    manager.addSend(&redis_send1);
    manager.addSend(&amqp_send1);
    manager.addSend(&mqtt_send);
    manager.startAll();

    std::this_thread::sleep_for(std::chrono::milliseconds(10000));

    manager.stopAll();
    manager.waitAll();

    return 0;
}
