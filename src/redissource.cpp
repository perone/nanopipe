#include <string>
#include <cstring>
#include <iostream>

#include "redissource.hpp"
#include "nanomessage.hpp"
#include "easylogging++.h"

#include <hiredis/async.h>
#include <hiredis/adapters/libuv.h>


RedisSource::RedisSource(const std::string &hostname,
                         const int port, const std::string &topic)
: NanoSource("RedisSource"), mRedisContext(nullptr), mHostname(hostname),
  mPort(port), mTopic(topic)
{ }


RedisSource::~RedisSource()
{ }


std::string RedisSource::hostname() const
{ return mHostname; }


int RedisSource::port() const
{ return mPort; }


std::string RedisSource::topic() const
{ return mTopic; }


void RedisSource::setHostname(std::string &hostname)
{ mHostname = hostname; }


void RedisSource::setPort(int port)
{ mPort = port; }


void RedisSource::setTopic(std::string &topic)
{ mTopic = topic; }


void RedisSource::onMessage(redisAsyncContext *c, void *reply, void *privdata)
{
    if (reply == nullptr || privdata == nullptr)
        return;

    RedisSource *this_node = static_cast<RedisSource*>(privdata);
    const redisReply *r = static_cast<redisReply*>(reply);

    if (r->type != REDIS_REPLY_ARRAY)
        return;

    if (r->elements < 3)
        return;
    
    const char *cmd_name = r->element[0]->str;
    const int cmd_name_len = r->element[0]->len;
    
    if (strncmp(cmd_name, "message", cmd_name_len) != 0)
        return;

    const char *topic_name = r->element[1]->str;
    const int topic_len = r->element[1]->len;
        
    if (strncmp(topic_name, this_node->mTopic.c_str(), topic_len) != 0)
        return;
    
    const char *data_content = r->element[2]->str;
    const int data_len = r->element[2]->len;

    auto new_message = \
        std::make_shared<NanoMessage>(data_content, data_len);
    this_node->publishMessage(new_message);
    
    return;
}


void RedisSource::onIdleEvent(uv_idle_t *handle)
{
    RedisSource *this_node = static_cast<RedisSource*>(handle->data);
    if (!this_node->shouldStop())
    {
        redisAsyncDisconnect(this_node->mRedisContext);
        uv_idle_stop(handle);
        uv_handle_t *uv_handle = \
            reinterpret_cast<uv_handle_t*>(handle);
        uv_close(uv_handle, nullptr);
    }
}

void RedisSource::onDisconnect(const redisAsyncContext *c,
                               int status)
{
    RedisSource *this_node = static_cast<RedisSource*>(c->data);
    if (status != REDIS_OK)
    {
        LOG(ERROR) << pname(this_node) << c->errstr;
        return;
    }
    
    LOG(INFO) << pname(this_node) << "disconnected.";
}


void RedisSource::onConnect(const redisAsyncContext *c,
                            int status)
{
    RedisSource *this_node = static_cast<RedisSource*>(c->data);

    if (status != REDIS_OK)
    {
        LOG(ERROR) << pname(this_node) << c->errstr;
        return;
    }
    LOG(INFO) << pname(this_node) << "connected.";
}


bool RedisSource::loop()
{   
    std::unique_ptr<uv_loop_t> uv_loop_redis(new uv_loop_t);
    uv_loop_init(uv_loop_redis.get());
    mRedisContext = redisAsyncConnect(mHostname.c_str(), mPort);

    if (mRedisContext == nullptr || mRedisContext->err)
    {
        if(mRedisContext)
            LOG(ERROR) << pname(this) << mRedisContext->errstr;
        else
            LOG(ERROR) << pname(this) << "cannot allocated Redis context.";

        return false;
    }

    const std::string subscribe_cmd = "SUBSCRIBE " + mTopic;

    mRedisContext->data = this;
    redisLibuvAttach(mRedisContext, uv_loop_redis.get());
    redisAsyncCommand(mRedisContext, RedisSource::onMessage, this, subscribe_cmd.c_str());
    redisAsyncSetConnectCallback(mRedisContext, RedisSource::onConnect);
    redisAsyncSetDisconnectCallback(mRedisContext, RedisSource::onDisconnect);

    uv_idle_t idler;
    idler.data = this;
    uv_idle_init(uv_loop_redis.get(), &idler);
    uv_idle_start(&idler, RedisSource::onIdleEvent);
    
    LOG(INFO) << pname(this) << "starting event loop.";
    uv_run(uv_loop_redis.get(), UV_RUN_DEFAULT);
    uv_loop_close(uv_loop_redis.get());
    
    return true;
}
