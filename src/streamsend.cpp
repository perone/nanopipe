#include "streamsend.hpp"
#include "easylogging++.h"

#include <string>

StreamSend::StreamSend(std::ostream *stream)
: NanoSend("StreamSend"), mStream(stream), mNewLine(true)
{ }

void StreamSend::setStream(std::ostream *stream)
{ mStream = stream; }


std::ostream *StreamSend::stream() const
{ return mStream; }


void StreamSend::setAddNewLine(bool add_new_line)
{ mNewLine = add_new_line; }


bool StreamSend::addNewLine() const
{ return mNewLine; }


void StreamSend::consumeMessage(NanoMessage::SharedMessage message)
{
    *mStream << message->buffer();
    if (mNewLine)
        *mStream << std::endl;
    return;
}

