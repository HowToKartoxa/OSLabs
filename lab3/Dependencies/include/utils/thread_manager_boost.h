#ifndef _THREAD_MANAGER_BOOST
#define _THREAD_MANAGER_BOOST

#include <utils/event_boost.h>

#include <boost/thread.hpp>

#include <vector>

class ThreadManager
{
public:
	ThreadManager(int*, size_t, unsigned short);
	~ThreadManager();
	void Operate();
	void PrintArray();

private:
	int* array_data;
	size_t array_size;

	unsigned short number_of_threads;
	unsigned short active_threads;

	boost::mutex output_mutex;
	boost::mutex array_mutex;

	Event start_threads_event;

	MultiEvent threads_stopped_events;
	MultiEvent* response_events;

	boost::thread** threads;

	void InitializeThreads();
	unsigned short GetThreadToKillIndex();
	void KillThread(unsigned short);
};

#endif