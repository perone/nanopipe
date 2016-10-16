#include "nanosend.hpp"


NanoSend::NanoSend(const std::string &send_name)
: NanoSched(send_name), mGracefulStop(true)
{ }


void NanoSend::addQueue(NanoMessage::SharedQueue queue)
{
    mMessageQueues.push_back(queue);
}


void NanoSend::setGracefulStop(bool graceful_stop)
{ mGracefulStop = graceful_stop; }


bool NanoSend::gracefulStop() const
{ return mGracefulStop; }


bool NanoSend::loop()
{
    while(shouldStop())
    {
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