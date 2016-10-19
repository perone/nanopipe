#ifndef NANOMESSAGE_HPP
#define NANOMESSAGE_HPP

#include <string>
#include <memory>

#include "producerconsumer.hpp"

using namespace folly;

class NanoMessage
{
public:
    typedef std::shared_ptr<NanoMessage> SharedMessage;
    typedef ProducerConsumerQueue<SharedMessage> SharedMessageQueue;
    typedef std::shared_ptr<NanoMessage::SharedMessageQueue> SharedQueue;

public:
    NanoMessage(const char *data_source, size_t size);
    NanoMessage();
    ~NanoMessage();

    // Disable copy and assign
    NanoMessage(const NanoMessage &other) = delete;
    NanoMessage &operator=(const NanoMessage &other) = delete;

public:
    std::string toString();

    char *buffer();
    size_t messageSize();

private:
    void copyFrom(const char *data_source, size_t size);

private:
    char *mMessageBuffer;
    size_t mMessageSize;
};

#endif
