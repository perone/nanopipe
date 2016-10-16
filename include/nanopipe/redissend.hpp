#ifndef REDISSEND_HPP
#define REDISSEND_HPP

#include <hiredis/hiredis.h>

#include "nanosend.hpp"
#include "nanomessage.hpp"

class RedisSend : public NanoSend
{
public:
    RedisSend(const std::string &hostname,
              const int port, const std::string &topic);

public:
    virtual bool initialize();
    virtual bool finish();
    
    virtual void consumeMessage(NanoMessage::SharedMessage message);

private:
    std::string mHostname;
    int mPort;
    std::string mTopic;
    redisContext *mRedisContext;
};

#endif
