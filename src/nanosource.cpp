#include "nanosource.hpp"

#include <iostream>

NanoSource::NanoSource(const std::string &source_name)
: NanoSched(source_name), mQueueSize(10)
{ }

void NanoSource::setQueueSize(int queue_size)
{ mQueueSize = queue_size; }

int NanoSource::queueSize() const
{ return mQueueSize; }

void NanoSource::addSend(NanoSend *send_node)
{
    NanoMessage::SharedQueue queue = \
        std::make_shared<NanoMessage::SharedMessageQueue>(mQueueSize);
        
    mMessageQueues.push_back(queue);
    send_node->addQueue(queue);
}

void NanoSource::publishMessage(NanoMessage::SharedMessage &message)
{
    if (mMessageQueues.empty())
        return;
    
    for(NanoMessage::SharedQueue &queue : mMessageQueues)
    {
        while (!queue->write(message))
            continue; // spin while queue is full
    }
}

