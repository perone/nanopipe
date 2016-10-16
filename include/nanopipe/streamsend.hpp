#ifndef STREAMSEND_HPP
#define STREAMSEND_HPP

#include "nanosend.hpp"
#include "nanomessage.hpp"

#include <ostream>

class StreamSend : public NanoSend
{
public:
    StreamSend(std::ostream *stream);

public:
    virtual void consumeMessage(NanoMessage::SharedMessage message);

public:
    void setStream(std::ostream *stream);
    std::ostream *stream() const;
    
    void setAddNewLine(bool add_new_line);
    bool addNewLine() const;

private:
    std::ostream *mStream;
    bool mNewLine;
};

#endif
