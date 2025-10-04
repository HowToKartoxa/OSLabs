#include <utils/marker_procedures.h>
#include <utils/marker_parameter_data.h>

#include <iostream>
#include <stack>

#include <windows.h>

DWORD WINAPI marker(LPVOID args) 
{
	MarkerParameterData data = *reinterpret_cast<MarkerParameterData*>(args);

	WaitForSingleObject(data.start_thread_event, INFINITE);

	srand(data.thread_index);

	size_t random;

	HANDLE response_events[2]{ data.exit_thread_event, data.start_thread_event };

	std::stack<size_t> marked_elements;

	while (true) 
	{
		random = rand() % data.array_size;
		if (data[random]) 
		{
			WaitForSingleObject(data.output_mutex, INFINITE);
			std::cout
				<< ">>> Thread "
				<< data.thread_index
				<< "\n    Marked "
				<< marked_elements.size()
				<< " elements so far"
				<< "\n    Cannot mark element at position "
				<< random;
			ReleaseMutex(data.output_mutex);

			SetEvent(data.thread_stopped_event);
			
			if (WaitForMultipleObjects(2, response_events, FALSE, INFINITE) == WAIT_OBJECT_0)
			{
				while (!marked_elements.empty()) 
				{
					data.array_data[marked_elements.top()] = 0;
					marked_elements.pop();
				}

				ExitThread(0);
			}
		}
		else 
		{
			Sleep(5);

			WaitForSingleObject(data.array_mutex, INFINITE);
			data[random] = data.thread_index;
			ReleaseMutex(data.array_mutex);

			Sleep(5);
		}
	}

}