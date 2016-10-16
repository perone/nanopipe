#ifndef NANOSEND_HPP
#define NANOSEND_HPP

#include <vector>

#include "nanosched.hpp"
#include "nanomessage.hpp"

class NanoSend : public NanoSched
{
public:
    NanoSend(const std::string &send_name);
    virtual ~NanoSend() { };

public:
    virtual void consumeMessage(NanoMessage::SharedMessage message) = 0;
    virtual void addQueue(NanoMessage::SharedQueue queue);
    virtual bool loop();
    
    virtual bool gracefulStop() const;
    virtual void setGracefulStop(bool graceful_stop);

protected:
    std::vector<NanoMessage::SharedQueue> mMessageQueues;
    bool mGracefulStop;
};

#endif
