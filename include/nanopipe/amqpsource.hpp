#ifndef AMQPSOURCE_HPP
#define AMQPSOURCE_HPP

#include <string>
#include <memory>

#include "nanosource.hpp"

#include <uv.h>
#include <amqpcpp.h>
#include <amqpcpp/libuv.h>


class AMQPSource : public NanoSource
{
public:
    AMQPSource(const std::string &hostname = "localhost",
               const int port = 5672,
               const std::string &queue = "myqueue",
               const std::string &login_user = "guest",
               const std::string &login_password = "guest",
               const std::string &vhost = "/");
    virtual ~AMQPSource();

public:
    static void onIdleEvent(uv_idle_t *handle);

public:
    bool loop();

    std::string hostname() const;
    int port() const;
    std::string queue() const;
    std::string loginUser() const;
    std::string vhost() const;
    
    void setHostname(const std::string &hostname);
    void setPort(int port);
    void setQueue(const std::string &queue_name);
    void setLoginUser(const std::string &login_user);
    void setLoginPassword(const std::string &login_password);

private:
    std::string mHostname;
    int mPort;
    std::string mQueue;
    std::string mLoginUser;
    std::string mLoginPassword;
    std::string mVhost;
    
    std::shared_ptr<AMQP::TcpConnection> mTcpConnection;
};

#endif