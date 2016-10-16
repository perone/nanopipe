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
    NanoMessage(const char *data_source, int size);
    NanoMessage();
    ~NanoMessage();

    // Disable copy and assign
    NanoMessage(const NanoMessage &other) = delete;
    NanoMessage &operator=(const NanoMessage &other) = delete;

public:
    std::string toString();

    char *buffer();
    int messageSize();

private:
    void copyFrom(const char *data_source, int size);

private:
    char *mMessageBuffer;
    int mMessageSize;
};

#endif
