#ifndef MQTTSOURCE_HPP
#define MQTTSOURCE_HPP

#include <string>
#include <vector>

#include <mosquitto.h>

#include "nanosource.hpp"

class MQTTSource : public NanoSource
{
public:
    MQTTSource(const std::string &hostname = "localhost",
               const int port = 1883,
               const std::string &topic = "mytopic",
               const int qos = 0);
    virtual ~MQTTSource();

public:
    static void onMessage(mosquitto *mosq, void *obj,
                          const mosquitto_message *message);
    static void onConnect(mosquitto *mosq, void *obj,
                          int result);
    static void onDisconnect(mosquitto *mosq, void *obj,
                             int result);

public:
    virtual bool loop();
    
    std::string hostname() const;
    int port() const;
    std::string topic() const;
    int qos() const;

    void setHostname(const std::string &hostname);
    void setPort(int port);
    void setTopic(const std::string &topic_name);
    void setQos(int qos);   
    

private:
    std::string mHostname;
    int mPort;
    std::string mTopic;
    int mQos;
    mosquitto *mMosquittoContext;
};

#endif