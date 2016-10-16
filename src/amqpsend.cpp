#include "amqpsend.hpp"
#include "easylogging++.h"

AMQPSend::AMQPSend(const std::string &hostname, const int port,
                   const std::string &exchange, const std::string &routing_key,
                   const std::string &login_user,
                   const std::string &login_password, const std::string &vhost)
: NanoSend("AMQPSend"), mHostname(hostname), mPort(port),
  mExchange(exchange), mRoutingKey(routing_key), mLoginUser(login_user),
  mLoginPassword(login_password), mVhost(vhost), mTcpConnection(nullptr),
  mTcpChannel(nullptr)
{ }


AMQPSend::~AMQPSend()
{ }


std::string AMQPSend::hostname() const
{ return mHostname; }


int AMQPSend::port() const
{ return mPort; }


std::string AMQPSend::exchange() const
{ return mExchange; }


std::string AMQPSend::routingKey() const
{ return mRoutingKey; }


std::string AMQPSend::loginUser() const
{ return mLoginUser; }
    

std::string AMQPSend::vhost() const
{ return mVhost; }


void AMQPSend::setHostname(const std::string &hostname)
{ mHostname = hostname; }


void AMQPSend::setPort(int port)
{ mPort = port; }


void AMQPSend::setExchange(const std::string &exchange_name)
{ mExchange = exchange_name; }


void AMQPSend::setRoutingKey(const std::string &routing_key)
{ mRoutingKey = routing_key; }


void AMQPSend::setLoginUser(const std::string &login_user)
{ mLoginUser = login_user; }


void AMQPSend::setLoginPassword(const std::string &login_password)
{ mLoginPassword = login_password; }


class NanoAMQPHandlerSend : public AMQP::LibUvHandler
{
public:
    NanoAMQPHandlerSend(uv_loop_t *loop);
    
public:
    virtual void onError(AMQP::TcpConnection *connection, const char *message);
    virtual void onConnected(AMQP::TcpConnection *connection);
    virtual void onClosed(AMQP::TcpConnection *connection);
    
private:
    uv_loop_t *mLoop;
};


void NanoAMQPHandlerSend::onConnected(AMQP::TcpConnection *connection)
{
    AMQPSend *this_node = static_cast<AMQPSend*>(mLoop->data);
    LOG(INFO) << NanoSched::pname(this_node) << "connected.";
    return;
}


void NanoAMQPHandlerSend::onClosed(AMQP::TcpConnection *connection)
{
    AMQPSend *this_node = static_cast<AMQPSend*>(mLoop->data);
    LOG(INFO) << NanoSched::pname(this_node) << "disconnected.";
    return;
}


void NanoAMQPHandlerSend::onError(AMQP::TcpConnection *connection, const char *message)
{
    AMQPSend *this_node = static_cast<AMQPSend*>(mLoop->data);
    LOG(ERROR) << NanoSched::pname(this_node) << message;
    return;
}


NanoAMQPHandlerSend::NanoAMQPHandlerSend(uv_loop_t *loop)
: AMQP::LibUvHandler(loop), mLoop(loop)
{ }


void AMQPSend::onIdleEvent(uv_idle_t *handle)
{
    AMQPSend *this_node = static_cast<AMQPSend*>(handle->data);
    if (!this_node->shouldStop())
    {
        this_node->mTcpConnection->close();
        uv_idle_stop(handle);
        uv_handle_t *uv_handle = \
            reinterpret_cast<uv_handle_t*>(handle);
        uv_close(uv_handle, nullptr);
    }
}


bool AMQPSend::loop()
{
    std::unique_ptr<uv_loop_t> uv_loop_amqp(new uv_loop_t);
    uv_loop_init(uv_loop_amqp.get());
    
    uv_loop_amqp->data = this;

    AMQP::Login login(mLoginUser, mLoginPassword);
    AMQP::Address address(mHostname, mPort, login, mVhost);

    NanoAMQPHandlerSend handler(uv_loop_amqp.get());
    
    mTcpConnection.reset(new AMQP::TcpConnection(&handler, address));
    mTcpChannel.reset(new AMQP::TcpChannel(mTcpConnection.get()));
    
    uv_idle_t idler;
    idler.data = this;
    uv_idle_init(uv_loop_amqp.get(), &idler);
    uv_idle_start(&idler, AMQPSend::onIdleEvent);
    
    LOG(INFO) << pname(this) << "starting event loop.";
    
    while (true)
    {
        const int ret = uv_run(uv_loop_amqp.get(), UV_RUN_NOWAIT);
        if (!ret)
            break;
            
        for (NanoMessage::SharedQueue &queue : mMessageQueues)
        {
            NanoMessage::SharedMessage message;
            const bool ret = queue->read(message);
            if(ret)
                consumeMessage(message);
        }
    }
    
    // Consume all messages in the queue after dying
    if(mGracefulStop)
    {
        for (NanoMessage::SharedQueue &queue : mMessageQueues)
        {
            NanoMessage::SharedMessage message;
            const bool ret = queue->read(message);
            if(ret)
                consumeMessage(message);
        }    
    }
    
    uv_loop_close(uv_loop_amqp.get());
    return true;
}


void AMQPSend::consumeMessage(NanoMessage::SharedMessage message)
{
    const char *buffer = message->buffer();
    const size_t buffer_size = message->messageSize();
    mTcpChannel->publish(mExchange, mRoutingKey, buffer, buffer_size);
    return;
}



