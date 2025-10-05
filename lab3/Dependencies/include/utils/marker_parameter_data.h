#ifndef _MARKER_PARAMETER_DATA
#define _MARKER_PARAMETER_DATA

#include <windows.h>

struct MarkerParameterData 
{
	int* array_data;

	size_t array_size;
	unsigned short thread_index;

	HANDLE output_mutex;
	HANDLE array_mutex;

	HANDLE start_threads_event;

	HANDLE exit_thread_event;
	HANDLE resume_thread_event;

	HANDLE thread_stopped_event;

	MarkerParameterData() :
		array_data(nullptr),
		array_size(0),
		thread_index(0),
		output_mutex(nullptr),
		array_mutex(nullptr),
		start_threads_event(nullptr),
		exit_thread_event(nullptr),
		resume_thread_event(nullptr),
		thread_stopped_event(nullptr)
	{
	}

	MarkerParameterData
	(
		int* _array_data,
		size_t _array_size,
		unsigned short _thread_index,
		HANDLE _output_mutex,
		HANDLE _array_mutex,
		HANDLE _start_threads_event,
		HANDLE _exit_thread_event,
		HANDLE _resume_thread_event,
		HANDLE _thread_stopped_event
	) :
		array_data(_array_data),
		array_size(_array_size),
		thread_index(_thread_index),
		output_mutex(_output_mutex),
		array_mutex(_array_mutex),
		start_threads_event(_start_threads_event),
		exit_thread_event(_exit_thread_event),
		resume_thread_event(_resume_thread_event),
		thread_stopped_event(_thread_stopped_event)
	{}

	int& operator[](const size_t& index) 
	{
		return array_data[index];
	}

	~MarkerParameterData() 
	{
		delete[] array_data;
	}
};

#endif