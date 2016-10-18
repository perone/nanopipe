#include "websocketsend.hpp"
#include "easylogging++.h"

WebSocketSend::WebSocketSend(const std::string &uri)
: NanoSend("WebSocketSend"), mUri(uri), mWSHub(nullptr)
{ }


WebSocketSend::~WebSocketSend()
{ }


std::string WebSocketSend::uri() const
{ return mUri; }


void WebSocketSend::setUri(const std::string &uri)
{ mUri = uri; }


void WebSocketSend::onIdleEvent(uv_idle_t *handle)
{
    WebSocketSend *this_node = static_cast<WebSocketSend*>(handle->data);
    if (!this_node->shouldStop())
    {
        LOG(INFO) << NanoSend::pname(this_node) << "closing event loop.";
        this_node->mWSHub->getDefaultGroup<uWS::CLIENT>();
        uv_idle_stop(handle);
        uv_handle_t *uv_handle = \
            reinterpret_cast<uv_handle_t*>(handle);
        uv_close(uv_handle, nullptr);
    }
}


bool WebSocketSend::loop()
{
    mWSHub.reset(new uWS::Hub());
    
    mWSHub->onConnection([this](uWS::WebSocket<uWS::CLIENT> ws,
                                uWS::UpgradeInfo ui)
    {
        LOG(INFO) << pname(this) << "connected.";
        this->mWSClient = ws;
    });

    mWSHub->onDisconnection([this](uWS::WebSocket<uWS::CLIENT> ws,
                                   int code, char *message, size_t length)
    {
        LOG(INFO) << pname(this) << "disconnected.";
    });

    mWSHub->onError([this](void *user)
    {
        LOG(ERROR) << pname(this) << "unknown error.";
    });

    mWSHub->connect(mUri.c_str(), nullptr);
    
    uv_loop_t *loop = mWSHub->getLoop();
    
    uv_idle_t idler;
    idler.data = this;
    uv_idle_init(loop, &idler);
    uv_idle_start(&idler, WebSocketSend::onIdleEvent);

    LOG(INFO) << pname(this) << "starting event loop.";
    
    while (true)
    {
        const int ret = uv_run(loop, UV_RUN_NOWAIT);
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
    
    return true;
}


void WebSocketSend::consumeMessage(NanoMessage::SharedMessage message)
{
    const char *buffer = message->buffer();
    const size_t buffer_size = message->messageSize();
    mWSClient.send(buffer, buffer_size, uWS::OpCode::TEXT);
    return;
}



