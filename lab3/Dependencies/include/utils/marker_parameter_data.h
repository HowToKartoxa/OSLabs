#ifndef _MARKER_PARAMETER_DATA
#define _MARKER_PARAMETER_DATA

#include <windows.h>

struct MarkerParameterData 
{
	int* array_data;

	size_t array_size;
	size_t thread_index;

	HANDLE output_mutex;

	HANDLE start_thread_event;
	HANDLE exit_thread_event;
	HANDLE resume_thread_event;

	HANDLE thread_stopped_event;

	HANDLE array_mutex;

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