#include "nanosched.hpp"

NanoSched::NanoSched(const std::string &sched_name)
: mSchedName(sched_name)
{ mStopFlag.test_and_set(); }

std::string NanoSched::name() const
{ return mSchedName; }

void NanoSched::setName(const std::string &name)
{ mSchedName = name; }

void NanoSched::signalStop()
{ mStopFlag.clear(); }

bool NanoSched::shouldStop()
{ return mStopFlag.test_and_set(); }

std::string NanoSched::pname(NanoSched *sched)
{ return "[" + sched->name() + "]: "; }

bool NanoSched::loop()
{ return true; }

bool NanoSched::finish()
{ return true; }

bool NanoSched::initialize()
{ return true; }
