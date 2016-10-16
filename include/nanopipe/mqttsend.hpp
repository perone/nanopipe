#ifndef MQTTSEND_HPP
#define MQTTSEND_HPP

#include "nanosend.hpp"
#include "nanomessage.hpp"

#include <mosquitto.h>

class MQTTSend : public NanoSend
{
public:
    MQTTSend(const std::string &hostname = "localhost",
             const int port = 1883,
             const std::string &topic = "mytopic",
             const int qos = 0);
    virtual ~MQTTSend();

public:
    virtual bool initialize();
    virtual bool finish();
    
    virtual void consumeMessage(NanoMessage::SharedMessage message);
    
public:
    std::string hostname() const;
    std::string topic() const;
    int port() const;
    int qos() const;
    
    void setHostname(const std::string &hostname);
    void setTopic(const std::string &topic);
    void setPort(int port);
    void setQos(int qos);
    
private:
    mosquitto *mMosquittoContext;
    std::string mHostname;
    std::string mTopic;
    int mPort;
    int mQos;
};

#endif
