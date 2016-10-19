#ifndef WEBSOCKETSOURCE_HPP
#define WEBSOCKETSOURCE_HPP

#include <string>
#include <vector>

#include <uv.h>
#include <uWS/uWS.h>

#include "nanosource.hpp"

class WebSocketSource : public NanoSource
{
public:
    WebSocketSource(const std::string &uri = "ws://localhost:8000");
    virtual ~WebSocketSource();

public:
    bool loop();

    std::string uri() const;
    void setUri(std::string &uri);

public:
    static void onIdleEvent(uv_idle_t *handle);

private:
    std::string mUri;
    
    std::shared_ptr<uWS::Hub> mWSHub;
};

#endif