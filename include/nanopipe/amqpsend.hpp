#ifndef AMQPSEND_HPP
#define AMQPSEND_HPP

#include "nanosend.hpp"
#include "nanomessage.hpp"

#include <memory>
#include <string>

#include <uv.h>
#include <amqpcpp.h>
#include <amqpcpp/libuv.h>

class AMQPSend : public NanoSend
{
public:
    AMQPSend(const std::string &hostname = "localhost",
             const int port = 5672,
             const std::string &exchange = "myexchange",
             const std::string &routing_key = "mykey",
             const std::string &login_user = "guest",
             const std::string &login_password = "guest",
             const std::string &vhost = "/");
    ~AMQPSend();

public:
    virtual void consumeMessage(NanoMessage::SharedMessage message);
    virtual bool loop();

public:
    std::string hostname() const;
    int port() const;
    std::string exchange() const;
    std::string routingKey() const;
    std::string loginUser() const;
    std::string vhost() const;

    void setHostname(const std::string &hostname);
    void setPort(int port);
    void setExchange(const std::string &exchange_name);
    void setRoutingKey(const std::string &routing_key);
    void setLoginUser(const std::string &login_user);
    void setLoginPassword(const std::string &login_password);

    static void onIdleEvent(uv_idle_t *handle);

private:
    std::string mHostname;
    int mPort;
    std::string mExchange;
    std::string mRoutingKey;
    std::string mLoginUser;
    std::string mLoginPassword;
    std::string mVhost;
    
    std::shared_ptr<AMQP::TcpConnection> mTcpConnection;
    std::shared_ptr<AMQP::TcpChannel> mTcpChannel;
};

#endif
