#include "redissend.hpp"
#include "easylogging++.h"

#include <iostream>

RedisSend::RedisSend(const std::string &hostname,
                     const int port, const std::string &topic)
: NanoSend("RedisSend"), mHostname(hostname), mPort(port),
  mTopic(topic), mRedisContext(nullptr)
{ }

bool RedisSend::initialize()
{
    mRedisContext = redisConnect(mHostname.c_str(), mPort);

    if (mRedisContext == nullptr || mRedisContext->err)
    {
        if(mRedisContext)
            LOG(ERROR) << pname(this) << mRedisContext->errstr;
        else
            LOG(ERROR) << pname(this) << "Cannot allocated Redis context.";

        return false;
    }

    LOG(INFO) << pname(this) << "connected.";
    
    return true;
}

bool RedisSend::finish()
{
    if(mRedisContext)
        redisFree(mRedisContext);
    return true;
}

void RedisSend::consumeMessage(NanoMessage::SharedMessage message)
{
    const char *buffer = message->buffer();
    const size_t buffer_size = message->messageSize();

    redisReply *reply = static_cast<redisReply*>(
        redisCommand(mRedisContext, "PUBLISH %s %b", mTopic.c_str(), buffer, buffer_size));
    
    freeReplyObject(reply);
    return;
}

