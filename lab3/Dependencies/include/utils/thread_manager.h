#ifndef _THREAD_MANAGER
#define _THREAD_MANAGER

#include <utils/marker_parameter_data.h>

#include <windows.h>
#include <string>

class ThreadManager 
{
public:
	ThreadManager(int*, size_t, unsigned short);
	~ThreadManager();
	DWORD Operate();
	void PrintArray();

private:
	int* array_data;
	size_t array_size;

	unsigned short number_of_threads;
	unsigned short active_threads;

	HANDLE output_mutex;
	HANDLE array_mutex;

	HANDLE* threads_stopped_events;
	HANDLE* threads;
	HANDLE* start_threads_events;

	DWORD* threads_ids;

	MarkerParameterData* threads_parameter_data;

	DWORD InitializeCommonHandles();
	DWORD InitializeThreads();
	DWORD HandleError(std::string);
	unsigned short GetThreadToKillIndex();
	void KillThread(unsigned short);
};

#endif