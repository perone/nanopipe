#include <string>
#include <cstring>
#include <mutex>
#include <chrono>

#include "mqttsource.hpp"
#include "nanomessage.hpp"
#include "easylogging++.h"

// mosquitto_lib_init() isn't thread-safe
std::mutex k_mqtt_lib_mutex;


MQTTSource::MQTTSource(const std::string &hostname,
                       const int port, const std::string &topic,
                       const int qos)
: NanoSource("MQTTSource"), mHostname(hostname), mPort(port),
  mTopic(topic), mQos(qos), mMosquittoContext(nullptr)
{ }


MQTTSource::~MQTTSource()
{ }

std::string MQTTSource::hostname() const
{ return mHostname; }


int MQTTSource::port() const
{ return mPort; }


std::string MQTTSource::topic() const
{ return mTopic; }


int MQTTSource::qos() const
{ return mQos; }


void MQTTSource::setHostname(const std::string &hostname)
{ mHostname = hostname; }


void MQTTSource::setPort(int port)
{ mPort = port; }


void MQTTSource::setTopic(const std::string &topic_name)
{ mTopic = topic_name; }


void MQTTSource::setQos(int qos)
{ mQos = qos; }


void MQTTSource::onMessage(mosquitto *mosq, void *obj,
                           const mosquitto_message *message)
{
    if (message == nullptr || obj == nullptr)
        return;

    MQTTSource *this_node = static_cast<MQTTSource*>(obj);
    
    const char *topic_name = message->topic;
    const char *payload = static_cast<const char*>(message->payload);
    const int payload_len = message->payloadlen;
    
    if (payload_len <= 0)
        return;
        
    if (strncmp(topic_name, this_node->mTopic.c_str(), payload_len) != 0)
        return;

    auto new_message = \
        std::make_shared<NanoMessage>(payload, payload_len);
    this_node->publishMessage(new_message);

    return;
}


void MQTTSource::onConnect(mosquitto *mosq, void *obj,
                           int result)
{
    MQTTSource *this_node = static_cast<MQTTSource*>(obj);
    
    switch(result)
    {
        case 0:    
            LOG(INFO) << pname(this_node) 
                      << "connected.";
            break;
        case 1:
            LOG(ERROR) << pname(this_node) 
                       << "connection refused (unacceptable protocol version)";
            break;
        case 2:
            LOG(ERROR) << pname(this_node) 
                       << "connection refused (identifier rejected)";
            break;
        case 3:
            LOG(ERROR) << pname(this_node) 
                       << "connection refused (broker unavailable)";
            break;
        default:
            LOG(ERROR) << pname(this_node) 
                       << "unknown error.";
            break;
    }
    
    return;
}


void MQTTSource::onDisconnect(mosquitto *mosq, void *obj,
                              int result)
{
    MQTTSource *this_node = static_cast<MQTTSource*>(obj);
    
    if (result == 0)
    {
        LOG(INFO) << pname(this_node) << "disconnected.";
        return;
    }
    
    LOG(ERROR) << pname(this_node) << "unexpected disconnection.";
    return;
}


bool MQTTSource::loop()
{   
    const bool clean_session = true;
    
    k_mqtt_lib_mutex.lock();
    mosquitto_lib_init();
    k_mqtt_lib_mutex.unlock();
    
    mMosquittoContext = mosquitto_new(NULL, clean_session, this);
    if(!mMosquittoContext)
    {
        LOG(ERROR) << pname(this) << "cannot allocate MQTT context.";
        return false;
    }
    
    mosquitto_message_callback_set(mMosquittoContext, MQTTSource::onMessage);
    mosquitto_connect_callback_set(mMosquittoContext, MQTTSource::onConnect);
    mosquitto_disconnect_callback_set(mMosquittoContext, MQTTSource::onDisconnect);
    
    if(mosquitto_connect(mMosquittoContext, mHostname.c_str(), mPort, 60))
    {
		LOG(ERROR) << pname(this) << "unable to connect to MQTT server.";
        mosquitto_destroy(mMosquittoContext);
        mosquitto_lib_cleanup();
		return false;
	}

    const int ret = mosquitto_subscribe(mMosquittoContext, NULL,
                                        mTopic.c_str(), mQos);
    if (ret != MOSQ_ERR_SUCCESS)
    {
		LOG(ERROR) << pname(this) << "cannot subscribe to topic, error: " << ret;
        mosquitto_disconnect(mMosquittoContext);
        mosquitto_destroy(mMosquittoContext);
        mosquitto_lib_cleanup();
		return false;
    }

    LOG(INFO) << pname(this) << "starting event loop.";
    while(shouldStop())
    {
        const int rc = mosquitto_loop(mMosquittoContext, -1, 1);
        if (rc == MOSQ_ERR_CONN_LOST || rc == MOSQ_ERR_NO_CONN)
        {
            LOG(ERROR) << pname(this) << "disconnected, trying to reconnect.";
            mosquitto_reconnect(mMosquittoContext);
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
            continue;
        }
        
        if (rc != MOSQ_ERR_SUCCESS)
        {
            const char *error_str = mosquitto_strerror(rc);
            LOG(ERROR) << pname(this) << "event loop error: " << error_str;
            mosquitto_disconnect(mMosquittoContext);
            mosquitto_destroy(mMosquittoContext);
            mosquitto_lib_cleanup();
            return false;
        }
    }

    mosquitto_disconnect(mMosquittoContext);
    mosquitto_destroy(mMosquittoContext);
    mosquitto_lib_cleanup();
    return true;
}



