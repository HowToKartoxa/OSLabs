#ifndef _MARKER_PARAMETER_DATA
#define _MARKER_PARAMETER_DATA

#include <utils/event_boost.h>

#include <boost/thread.hpp>

#include <windows.h>

struct MarkerParameterData 
{
	int* array_data;

	size_t array_size;
	unsigned short thread_number;

	HANDLE output_mutex;
	HANDLE array_mutex;

	HANDLE start_threads_event;

	HANDLE exit_thread_event;

	HANDLE thread_stopped_event;

	MarkerParameterData() :
		array_data(nullptr),
		array_size(0),
		thread_number(0),
		output_mutex(nullptr),
		array_mutex(nullptr),
		start_threads_event(nullptr),
		exit_thread_event(nullptr),
		thread_stopped_event(nullptr)
	{
	}

	int& operator[](const size_t& index) 
	{
		return array_data[index];
	}

	MarkerParameterData& operator=(const MarkerParameterData& src) 
	{
		array_data = src.array_data;
		array_size = src.array_size;
		thread_number = src.thread_number;
		output_mutex = src.output_mutex;
		array_mutex = src.array_mutex;
		start_threads_event = src.start_threads_event;
		exit_thread_event = src.exit_thread_event;
		thread_stopped_event = src.thread_stopped_event;
		return *this;
	}
};

#endif