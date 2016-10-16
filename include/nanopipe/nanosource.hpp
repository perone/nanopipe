#ifndef NANOSOURCE_HPP
#define NANOSOURCE_HPP

#include "nanosend.hpp"
#include "nanomessage.hpp"
#include "nanosched.hpp"

#include <vector>
#include <memory>

class NanoSource : public NanoSched
{
public:
    NanoSource(const std::string &source_name);
    virtual ~NanoSource() {};

public:
    virtual void addSend(NanoSend *send_node);
    virtual void publishMessage(NanoMessage::SharedMessage &message);
    virtual void setQueueSize(int queue_size);
    virtual int queueSize() const;

private:
    std::vector<NanoMessage::SharedQueue> mMessageQueues;
    int mQueueSize;
};

#endif