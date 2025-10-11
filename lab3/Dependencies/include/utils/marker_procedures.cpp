#include <utils/marker_procedures.h>
#include <utils/marker_parameter_data.h>

#include <iostream>
#include <stack>

#include <windows.h>

DWORD WINAPI marker(LPVOID args) 
{
	MarkerParameterData& data = *reinterpret_cast<MarkerParameterData*>(args);

	WaitForSingleObject(data.start_threads_event, INFINITE);

	srand(data.thread_number);

	size_t random;

	HANDLE response_events[2]{ data.exit_thread_event, data.start_threads_event };

	std::stack<size_t> marked_elements;

	while (true) 
	{
		random = rand() % data.array_size;
		if (data.array_data[random]) 
		{
			WaitForSingleObject(data.output_mutex, INFINITE);
			std::cout
				<< "--> Thread ("
				<< data.thread_number
				<< "):\n    "
				<< marked_elements.size()
				<< " marked elements so far"
				<< "\n    Cannot mark element at position "
				<< random
				<< '\n';
			ReleaseMutex(data.output_mutex);

			SetEvent(data.thread_stopped_event);
			
			if (WaitForMultipleObjects(2, response_events, FALSE, INFINITE) == WAIT_OBJECT_0)
			{
				while (!marked_elements.empty()) 
				{
					data[marked_elements.top()] = 0;
					marked_elements.pop();
				}
				break;
			}
		}
		else 
		{
			Sleep(5);

			WaitForSingleObject(data.array_mutex, INFINITE);
			data[random] = data.thread_number;
			ReleaseMutex(data.array_mutex);

			Sleep(5);

			marked_elements.push(random);
		}
	}
	return 0;
}