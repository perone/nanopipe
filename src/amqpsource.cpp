#include "amqpsource.hpp"
#include "nanomessage.hpp"
#include "easylogging++.h"


AMQPSource::AMQPSource(const std::string &hostname, const int port,
                       const std::string &queue, const std::string &login_user,
                       const std::string &login_password, const std::string &vhost)
: NanoSource("AMPQSource"), mHostname(hostname), mPort(port),
  mQueue(queue), mLoginUser(login_user), mLoginPassword(login_password),
  mVhost(vhost)
{ }


AMQPSource::~AMQPSource()
{ }


std::string AMQPSource::hostname() const
{ return mHostname; }


int AMQPSource::port() const
{ return mPort; }


std::string AMQPSource::queue() const
{ return mQueue; }


std::string AMQPSource::loginUser() const
{ return mLoginUser; }
    

std::string AMQPSource::vhost() const
{ return mVhost; }


void AMQPSource::setHostname(const std::string &hostname)
{ mHostname = hostname; }


void AMQPSource::setPort(int port)
{ mPort = port; }


void AMQPSource::setQueue(const std::string &queue_name)
{ mQueue = queue_name; }


void AMQPSource::setLoginUser(const std::string &login_user)
{ mLoginUser = login_user; }


void AMQPSource::setLoginPassword(const std::string &login_password)
{ mLoginPassword = login_password; }


class NanoAMQPHandler : public AMQP::LibUvHandler
{
public:
    virtual void onError(AMQP::TcpConnection *connection, const char *message);
    virtual void onConnected(AMQP::TcpConnection *connection);
    virtual void onClosed(AMQP::TcpConnection *connection);

public:
    NanoAMQPHandler(uv_loop_t *loop);
    
private:
    uv_loop_t *mLoop;
};


void NanoAMQPHandler::onConnected(AMQP::TcpConnection *connection)
{
    AMQPSource *this_node = static_cast<AMQPSource*>(mLoop->data);
    LOG(INFO) << NanoSched::pname(this_node) << "connected.";
    return;
}


void NanoAMQPHandler::onClosed(AMQP::TcpConnection *connection)
{
    AMQPSource *this_node = static_cast<AMQPSource*>(mLoop->data);
    LOG(INFO) << NanoSched::pname(this_node) << "disconnected.";
    return;
}


void NanoAMQPHandler::onError(AMQP::TcpConnection *connection, const char *message)
{
    AMQPSource *this_node = static_cast<AMQPSource*>(mLoop->data);
    LOG(ERROR) << NanoSched::pname(this_node) << message;
    return;
}


NanoAMQPHandler::NanoAMQPHandler(uv_loop_t *loop)
: AMQP::LibUvHandler(loop), mLoop(loop)
{ }


void AMQPSource::onIdleEvent(uv_idle_t *handle)
{
    AMQPSource *this_node = static_cast<AMQPSource*>(handle->data);
    if (!this_node->shouldStop())
    {
        this_node->mTcpConnection->close();
        uv_idle_stop(handle);
        uv_handle_t *uv_handle = \
            reinterpret_cast<uv_handle_t*>(handle);
        uv_close(uv_handle, nullptr);
    }
}


bool AMQPSource::loop()
{   
    std::unique_ptr<uv_loop_t> uv_loop_amqp(new uv_loop_t);
    uv_loop_init(uv_loop_amqp.get());
    
    uv_loop_amqp->data = this;

    AMQP::Login login(mLoginUser, mLoginPassword);
    AMQP::Address address(mHostname, mPort, login, mVhost);

    NanoAMQPHandler handler(uv_loop_amqp.get());
    
    mTcpConnection.reset(new AMQP::TcpConnection(&handler, address));
    AMQP::TcpChannel channel(mTcpConnection.get());
   
    auto message_callback = [&channel,this](const AMQP::Message &message,
                                       uint64_t deliveryTag, bool redelivered)
    {
        auto new_message = \
            std::make_shared<NanoMessage>(message.body(), message.bodySize());
        this->publishMessage(new_message);
        channel.ack(deliveryTag);
    };
    
    auto error_callback = [this](const char *message)
    {
        LOG(ERROR) << pname(this) << message;
        return;
    };
        
    channel.consume(mQueue)
        .onReceived(message_callback)
        .onError(error_callback);

    uv_idle_t idler;
    idler.data = this;
    uv_idle_init(uv_loop_amqp.get(), &idler);
    uv_idle_start(&idler, AMQPSource::onIdleEvent);
    
    LOG(INFO) << pname(this) << "starting event loop.";
    uv_run(uv_loop_amqp.get(), UV_RUN_DEFAULT);
    uv_loop_close(uv_loop_amqp.get());

    return true;
}



