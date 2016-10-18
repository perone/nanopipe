#ifndef WEBSOCKETSEND_HPP
#define WEBSOCKETSEND_HPP

#include "nanosend.hpp"
#include "nanomessage.hpp"

#include <string>

#include <uv.h>
#include <uWS/uWS.h>

class WebSocketSend : public NanoSend
{
public:
    WebSocketSend(const std::string &uri = "ws://localhost:8000");
    ~WebSocketSend();

public:
    virtual void consumeMessage(NanoMessage::SharedMessage message);
    virtual bool loop();

public:
    std::string uri() const;
    
    void setUri(const std::string &uri);
    static void onIdleEvent(uv_idle_t *handle);

private:
    std::string mUri;
    std::shared_ptr<uWS::Hub> mWSHub;
    uWS::WebSocket<uWS::CLIENT> mWSClient;
};

#endif
