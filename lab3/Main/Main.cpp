#include <utils/input_parsing.h>
#include <utils/marker_procedures.h>
#include <utils/marker_parameter_data.h>

#include <iostream>
#include <string>

#include <windows.h>

int main(int arc, char** argv) 
{
	std::string temp_string;

	std::cout << "Enter array size:\n";
	std::getline(std::cin, temp_string);
	while (!CheckIfSizeT(temp_string))
	{
		std::cout << "Enter array size:\n";
		std::getline(std::cin, temp_string);
	}

	size_t size = StringToSizeT(temp_string);
	int* array = new int[size];
	for (size_t i = 0; i < size; i++) 
	{
		array[i] = 0;
	}

	std::cout << "Enter number of marker threads:\n";
	std::getline(std::cin, temp_string);
	while (!CheckIfUnsignedShort(temp_string)) 
	{
		std::cout << "Enter number of marker threads:\n";
		std::getline(std::cin, temp_string);
	}

	unsigned short number_of_threads = std::stoi(temp_string);

	HANDLE output_mutex = CreateMutexA(NULL, FALSE, "OUTPUT_MUTEX");
	if (output_mutex == NULL) 
	{
		DWORD error = GetLastError();
		std::cout << "Failed to create output mutex with error: " << error << " !\n";
		std::system("pause");
		return error;
	}

	HANDLE array_mutex = CreateMutexA(NULL, FALSE, "ARRAY_MUTEX");
	if (array_mutex == NULL) 
	{
		DWORD error = GetLastError();
		std::cout << "Failed to create array mutex with error: " << error << " !\n";
		std::system("pause");
		return error;
	}

	HANDLE start_threads_event = CreateEventA(NULL, TRUE, FALSE, "START_THREADS_EVENT");
	if (start_threads_event == NULL) 
	{
		DWORD error = GetLastError();
		std::cout << "Failed to create start threads event with error: " << error << " !\n";
		std::system("pause");
		return error;
	}

	HANDLE* thread_stopped_events = new HANDLE[number_of_threads];

	HANDLE* threads = new HANDLE[number_of_threads];
	DWORD* thread_ids = new DWORD[number_of_threads];

	MarkerParameterData* threads_parameter_data = new MarkerParameterData[number_of_threads];

	for (unsigned short i = 0; i < number_of_threads; i++) 
	{
		threads_parameter_data[i].array_data = array;
		threads_parameter_data[i].array_size = size;
		threads_parameter_data[i].thread_number = i + 1;
		threads_parameter_data[i].output_mutex = output_mutex;
		threads_parameter_data[i].array_mutex = array_mutex;
		threads_parameter_data[i].start_threads_event = start_threads_event;

		thread_stopped_events[i] = CreateEventA(NULL, TRUE, FALSE, ("THREAD_" + std::to_string(i) + "_STOPPED").c_str());
		if (thread_stopped_events[i] == NULL)
		{
			DWORD error = GetLastError();
			std::cout << "Failed to create thread stopped event for thread number " << i << " with error: " << error << " !\n";
			std::system("pause");
			return error;
		}
		threads_parameter_data[i].thread_stopped_event = thread_stopped_events[i];

		threads_parameter_data[i].exit_thread_event = CreateEventA(NULL, TRUE, FALSE, ("EXIT_THREAD_" + std::to_string(i)).c_str());
		if (threads_parameter_data[i].exit_thread_event == NULL)
		{
			DWORD error = GetLastError();
			std::cout << "Failed to create exit thread event for thread number " << i << " with error: " << error << " !\n";
			std::system("pause");
			return error;
		}
		
		threads[i] = CreateThread(NULL, NULL, marker, reinterpret_cast<LPVOID*>(&threads_parameter_data[i]), NULL, &thread_ids[i]);
		if (threads[i] == NULL) 
		{
			DWORD error = GetLastError();
			std::cout << "Failed to create thread number " << i << " with error: " << error << " !\n";
			std::system("pause");
			return error;
		}
	}

	Sleep(25 * number_of_threads);

	PulseEvent(start_threads_event);

	unsigned short thread_to_kill_number;
	unsigned short thread_to_kill_index;
	unsigned short active_threads_left = number_of_threads;
	bool invalid_data_entered = true;

	MarkerParameterData temp_marker_parameter_data;
	HANDLE temp_handle;

	DWORD wait_result;

	while (active_threads_left) 
	{
		wait_result = WaitForMultipleObjects(number_of_threads, thread_stopped_events, TRUE, INFINITE);

		if (wait_result >= WAIT_ABANDONED_0 && wait_result <= WAIT_ABANDONED_0 + number_of_threads - 1) 
		{
			DWORD error = GetLastError();
			std::cout << "Failed to wait for marker threads to stop with error: " << error << " !\n";
			std::system("pause");
			return error;
		}

		WaitForSingleObject(output_mutex, INFINITE);

		std::cout << "\nArray after all marker threads stopped:\n";
		for (size_t i = 0; i < size; i++)
		{
			std::cout << array[i] << ' ';
		}
		std::cout << '\n';

		while (true) 
		{
			std::cout << "\nEnter the number of marker thread to kill:\n";
			std::getline(std::cin, temp_string);
			while (!CheckIfUnsignedShort(temp_string))
			{
				std::cout << "Enter the number of marker thread to kill:\n";
				std::getline(std::cin, temp_string);
			}
			
			thread_to_kill_number = std::stoi(temp_string);
			for (unsigned short i = 0; i < number_of_threads; i++) 
			{
				if (threads_parameter_data[i].array_data != nullptr && threads_parameter_data[i].thread_number == thread_to_kill_number)
				{
					thread_to_kill_index = i;
					invalid_data_entered = false;
					break;
				}
			}
			if (invalid_data_entered) 
			{
				std::cout << "Thread of this number does not exist!\n";
			}
			else 
			{
				break;
			}
		}
		invalid_data_entered = true;

		ReleaseMutex(output_mutex);

		SetEvent(threads_parameter_data[thread_to_kill_index].exit_thread_event);
		wait_result = WaitForSingleObject(threads[thread_to_kill_index], INFINITE);
		if (wait_result == WAIT_FAILED)
		{
			DWORD error = GetLastError();
			std::cout << "Failed to wait for marker thread of number " << thread_to_kill_number << " to finish with error: " << error << " !\n";
			std::system("pause");
			return error;
		}

		WaitForSingleObject(output_mutex, INFINITE);

		std::cout << "\nArray after marker thread of number " << thread_to_kill_number << " exited:\n";
		for (size_t i = 0; i < size; i++)
		{
			std::cout << array[i] << ' ';
		}
		std::cout << '\n';

		ReleaseMutex(output_mutex);

		CloseHandle(threads[thread_to_kill_index]);
		CloseHandle(threads_parameter_data[thread_to_kill_index].exit_thread_event);
		CloseHandle(threads_parameter_data[thread_to_kill_index].thread_stopped_event);

		threads_parameter_data[thread_to_kill_index].array_data = nullptr;

		active_threads_left--;
		PulseEvent(start_threads_event);

		Sleep(25 * number_of_threads);
	}

	delete[] array;
	delete[] thread_stopped_events;
	delete[] threads;
	delete[] threads_parameter_data;

	system("pause");
	return 0;
}