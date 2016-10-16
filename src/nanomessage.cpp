#include <cstring>
#include <iostream>

#include "nanomessage.hpp"

NanoMessage::NanoMessage()
: mMessageBuffer(nullptr), mMessageSize(0)
{ }

NanoMessage::~NanoMessage()
{ 
    if (mMessageBuffer != nullptr)
        delete [] mMessageBuffer;
}

NanoMessage::NanoMessage(const char *data_source, int size)
: NanoMessage()
{
    copyFrom(data_source, size);
}

void NanoMessage::copyFrom(const char *data_source, int size)
{
    if (mMessageBuffer)
        delete [] mMessageBuffer;

    mMessageBuffer = new char[size];
    std::memcpy(mMessageBuffer, data_source, sizeof(char) * size);
    mMessageSize = size;
}

std::string NanoMessage::toString()
{
    const std::string message_text(mMessageBuffer, mMessageSize);
    return message_text;
}

char *NanoMessage::buffer()
{ return mMessageBuffer; }

int NanoMessage::messageSize()
{ return mMessageSize; }

