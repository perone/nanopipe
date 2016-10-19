#include "websocketsource.hpp"
#include "nanomessage.hpp"
#include "easylogging++.h"


WebSocketSource::WebSocketSource(const std::string &uri)
: NanoSource("WebSocketSource"), mUri(uri), mWSHub(nullptr)
{ }


WebSocketSource::~WebSocketSource()
{ }


std::string WebSocketSource::uri() const
{ return mUri; }


void WebSocketSource::setUri(std::string &uri)
{ mUri = uri; }


void WebSocketSource::onIdleEvent(uv_idle_t *handle)
{
    WebSocketSource *this_node = static_cast<WebSocketSource*>(handle->data);
    if (!this_node->shouldStop())
    {
        this_node->mWSHub->getDefaultGroup<uWS::CLIENT>();
        uv_idle_stop(handle);
        uv_handle_t *uv_handle = \
            reinterpret_cast<uv_handle_t*>(handle);
        uv_close(uv_handle, nullptr);
    }
}


bool WebSocketSource::loop()
{   
    mWSHub.reset(new uWS::Hub());
    mWSHub->onMessage([this](uWS::WebSocket<uWS::CLIENT> ws,
                       char *message, size_t length,
                       uWS::OpCode opCode)
    {
        auto new_message = \
            std::make_shared<NanoMessage>(message, length);
        this->publishMessage(new_message);
    });
    
    mWSHub->onConnection([this](uWS::WebSocket<uWS::CLIENT> ws,
                                uWS::UpgradeInfo ui)
    {
        LOG(INFO) << pname(this) << "connected.";
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
    
    uv_idle_t idler;
    idler.data = this;
    uv_idle_init(mWSHub->getLoop(), &idler);
    uv_idle_start(&idler, WebSocketSource::onIdleEvent);

    LOG(INFO) << pname(this) << "starting event loop.";
    mWSHub->run();
    return true;
}
