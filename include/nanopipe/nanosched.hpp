#ifndef NANOSCHED_HPP
#define NANOSCHED_HPP

#include <atomic>
#include <string>

class NanoSched
{
public:
    NanoSched(const std::string &sched_name);

    NanoSched(const NanoSched&) = delete;
    NanoSched& operator=(const NanoSched&) = delete;
    NanoSched() = delete;

    virtual ~NanoSched() {};

public:
    virtual bool loop();
    
    virtual bool initialize();
    virtual bool finish();
    
    virtual void signalStop();
    virtual std::string name() const;
    virtual void setName(const std::string &name);
    virtual bool shouldStop();
    
    static std::string pname(NanoSched *sched);
    
private:
    std::atomic_flag mStopFlag;
    std::string mSchedName;
};

#endif 