#include "mqttsend.hpp"
#include "easylogging++.h"

#include <mutex>

// mosquitto_lib_init() isn't thread-safe
extern std::mutex k_mqtt_lib_mutex;


MQTTSend::MQTTSend(const std::string &hostname,
                   const int port, const std::string &topic,
                   const int qos)
: NanoSend("MQTTSend"),
  mMosquittoContext(nullptr), mHostname(hostname),
  mTopic(topic), mPort(port), mQos(qos)
{ }


MQTTSend::~MQTTSend()
{ }


std::string MQTTSend::hostname() const
{ return mHostname; }


std::string MQTTSend::topic() const
{ return mTopic; }


int MQTTSend::port() const
{ return mPort; }


int MQTTSend::qos() const
{ return mQos; }


void MQTTSend::setHostname(const std::string &hostname)
{ mHostname = hostname; }


void MQTTSend::setTopic(const std::string &topic)
{ mTopic = topic; }


void MQTTSend::setPort(int port)
{ mPort = port; }


void MQTTSend::setQos(int qos)
{ mQos = qos; }


bool MQTTSend::initialize()
{
    const bool clean_session = true;
    
    k_mqtt_lib_mutex.lock();
    mosquitto_lib_init();
    k_mqtt_lib_mutex.unlock();
    
    mMosquittoContext = mosquitto_new(NULL, clean_session, NULL);
    if(!mMosquittoContext)
    {
        LOG(ERROR) << pname(this) << "out of memory.";
        return false;
    }
    
    if(mosquitto_connect(mMosquittoContext, mHostname.c_str(), mPort, 60))
    {
        LOG(ERROR) << pname(this) << "unable to connect.";
		return false;
	}
    return true;
}


bool MQTTSend::finish()
{
    if (mMosquittoContext)
    {
        mosquitto_disconnect(mMosquittoContext);
        mosquitto_destroy(mMosquittoContext);
    }   
    
    mosquitto_lib_cleanup();
    return true;
}


void MQTTSend::consumeMessage(NanoMessage::SharedMessage message)
{
    const char *buffer = message->buffer();
    const size_t buffer_size = message->messageSize();

    const int ret = \
        mosquitto_publish(mMosquittoContext, NULL, mTopic.c_str(),
                           buffer_size, buffer, mQos, false);

    if (ret)
    {
        LOG(ERROR) << "unable to publish message.";
        return;
    }
    
    return;
}

