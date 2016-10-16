#ifndef REDISSOURCE_HPP
#define REDISSOURCE_HPP

#include <string>
#include <vector>

#include <hiredis/async.h>
#include <hiredis/adapters/libuv.h>

#include "nanosource.hpp"

class RedisSource : public NanoSource
{
public:
    RedisSource(const std::string &hostname = "localhost",
                const int port = 6379,
                const std::string &topic = "mytopic");
    virtual ~RedisSource();

public:
    static void onMessage(redisAsyncContext *c, void *reply,
                          void *privdata);
    static void onDisconnect(const redisAsyncContext *c,
                             int status);
    static void onConnect(const redisAsyncContext *c,
                          int status);
    static void onIdleEvent(uv_idle_t *handle);

public:
    bool loop();
    std::string hostname() const;
    int port() const;
    std::string topic() const;

    void setHostname(std::string &hostname);
    void setPort(int port);
    void setTopic(std::string &topic);

private:
    redisAsyncContext *mRedisContext;
    std::string mHostname;
    int mPort;
    std::string mTopic;
};

#endif