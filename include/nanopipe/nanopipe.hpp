/** @file */

#ifndef NANOPIPE_HPP
#define NANOPIPE_HPP

#include <vector>
#include <thread>

#include "nanosched.hpp"
#include "nanomessage.hpp"
#include "nanosend.hpp"
#include "nanosend.hpp"
#include "nanosource.hpp"
#include "producerconsumer.hpp"
#include "nanosource.hpp"
#include "redissource.hpp"
#include "streamsend.hpp"
#include "redissend.hpp"
#include "mqttsend.hpp"
#include "mqttsource.hpp"
#include "amqpsource.hpp"
#include "amqpsend.hpp"

/**
 * @brief Returns the major version of the library.
 * @return The major version of the Nanopipe library.
 */
int nanopipe_major_version();

int nanopipe_minor_version();
int nanopipe_patch_version();

const char* nanopipe_git_branch();
const char* nanopipe_git_commit_hash();

void nanopipe_init();

class NanoManager
{
public:
	NanoManager();
	~NanoManager();

public:
    /**
     * @brief Adds a new NanoSource connection.
     * @param nano_source The connection to add.
     */
	void addSource(NanoSource *nano_source);
	void addSend(NanoSend *nano_send);

public:
	void startAll();
	void stopAll();
	void waitAll();

private:
	std::vector<NanoSched*> mNanoSources;
	std::vector<NanoSched*> mNanoSends;
	
	std::vector<std::thread> mThreadSources;
	std::vector<std::thread> mThreadSends;
};

class SchedManager
{
public:
    SchedManager(NanoSched *sched);

public:
    void run();
 
private:
    NanoSched *mSched;
    
};

#endif