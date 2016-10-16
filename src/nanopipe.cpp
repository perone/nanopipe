#include "nanopipe.hpp"
#include "nanopipe_version.hpp"
#include "easylogging++.h"

INITIALIZE_EASYLOGGINGPP

int nanopipe_major_version()
{ return NANOPIPE_VERSION_MAJOR; }

int nanopipe_minor_version()
{ return NANOPIPE_VERSION_MINOR; }

int nanopipe_patch_version()
{ return NANOPIPE_VERSION_PATCH; }

const char* nanopipe_git_branch()
{ return NANOPIPE_GIT_BRANCH; }

const char* nanopipe_git_commit_hash()
{ return NANOPIPE_GIT_COMMIT_HASH; }

void nanopipe_init()
{
    el::Configurations defaultConf;
    defaultConf.setToDefault();
    defaultConf.setGlobally(
        el::ConfigurationType::Format,
        //"%func %datetime %level %msg");
        "%datetime %level %msg");
    el::Loggers::reconfigureLogger("default", defaultConf);
    el::Loggers::addFlag(el::LoggingFlag::ColoredTerminalOutput);
    return;
}

SchedManager::SchedManager(NanoSched *sched)
: mSched(sched)
{ }

void SchedManager::run()
{
    const std::string sched_name = mSched->name();
    
    const bool init_ret = mSched->initialize();
    if (!init_ret)
    {
        LOG(ERROR) << "Thread [" << std::this_thread::get_id()
                   << ":" << sched_name 
                   << "] initialization failed.";
    }    

    const bool thread_ret = mSched->loop();
    if (!thread_ret)
    {
        LOG(ERROR) << "Thread [" << std::this_thread::get_id()
                   << ":" << sched_name 
                   << "] loop finished with errors.";
    }
    
    const bool finish_ret = mSched->finish();
    if (!finish_ret)
    {
        LOG(ERROR) << "Thread [" << std::this_thread::get_id()
                   << ":" << sched_name 
                   << "] finished with errors.";
    }
    
    return;
}

NanoManager::NanoManager()
{ }

NanoManager::~NanoManager()
{ }

void NanoManager::addSource(NanoSource *nano_source)
{ mNanoSources.push_back(nano_source); }

void NanoManager::addSend(NanoSend *nano_send)
{ mNanoSends.push_back(nano_send); }

void NanoManager::startAll()
{
    for(NanoSched *sched : mNanoSends)
    {
        SchedManager mm(sched);
        std::thread thread(&SchedManager::run, mm);
        mThreadSends.push_back(std::move(thread));
    }

    for(NanoSched *sched : mNanoSources)
    {
        SchedManager mm(sched);
        std::thread thread(&SchedManager::run, mm);
        mThreadSources.push_back(std::move(thread));
    }
}

void NanoManager::stopAll()
{
    for(NanoSched *sched : mNanoSources)
    	sched->signalStop();

    for(NanoSched *sched : mNanoSends)
    	sched->signalStop();
}

void NanoManager::waitAll()
{
    for(std::thread &thread : mThreadSources)
        thread.join();
    
    for(std::thread &thread : mThreadSends)
        thread.join();
}