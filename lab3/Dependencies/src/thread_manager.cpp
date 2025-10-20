#include <utils/thread_manager.h>
#include <utils/input_parsing.h>
#include <utils/marker_procedures.h>

#include <iostream>

ThreadManager::ThreadManager(int* _array_data, size_t _array_size, unsigned short _thread_num) 
{
	array_data = _array_data;
	array_size = _array_size;
	number_of_threads = _thread_num;
	active_threads = 0;

	DWORD error;

	if(error = InitializeCommonHandles()) throw error;
	if(error = InitializeThreads()) throw error;
}

DWORD ThreadManager::InitializeCommonHandles() 
{
	output_mutex = CreateMutexA(NULL, FALSE, "OUTPUT_MUTEX");
	if (output_mutex == NULL)
	{
		return HandleError("failed to create output mutex!");
	}

	array_mutex = CreateMutexA(NULL, FALSE, "ARRAY_MUTEX");
	if (array_mutex == NULL)
	{
		return HandleError("failed to create array mutex!");
	}

	start_threads_event = CreateEventA(NULL, TRUE, FALSE, "START_THREADS_EVENT");
	if (start_threads_event == NULL)
	{
		return HandleError("failed to create start threads event!");
	}

	return 0;
}

DWORD ThreadManager::InitializeThreads() 
{
	threads_stopped_events = new HANDLE[number_of_threads];
	threads = new HANDLE[number_of_threads];
	threads_ids = new DWORD[number_of_threads];

	threads_parameter_data = new MarkerParameterData[number_of_threads];

	for (unsigned short i = 0; i < number_of_threads; i++)
	{
		threads_parameter_data[i].array_data = array_data;
		threads_parameter_data[i].array_size = array_size;
		threads_parameter_data[i].thread_number = i + 1;
		threads_parameter_data[i].output_mutex = output_mutex;
		threads_parameter_data[i].array_mutex = array_mutex;
		threads_parameter_data[i].start_threads_event = start_threads_event;

		threads_stopped_events[i] = CreateEventA(NULL, TRUE, FALSE, ("THREAD_" + std::to_string(i) + "_STOPPED").c_str());
		if (threads_stopped_events[i] == NULL)
		{
			return HandleError(std::string("failed to create thread stopped event for thread " + i + '!'));
		}

		threads_parameter_data[i].thread_stopped_event = threads_stopped_events[i];

		threads_parameter_data[i].exit_thread_event = CreateEventA(NULL, TRUE, FALSE, ("EXIT_THREAD_" + std::to_string(i)).c_str());
		if (threads_parameter_data[i].exit_thread_event == NULL)
		{
			return HandleError(std::string("failed to create exit thread event for thread " + i + '!'));
		}

		threads[i] = CreateThread(NULL, NULL, marker, reinterpret_cast<LPVOID*>(&threads_parameter_data[i]), NULL, &threads_ids[i]);
		if (threads[i] == NULL)
		{
			return HandleError(std::string("failed to create thread " + i + '!'));
		}
	}

	return 0;
}

DWORD ThreadManager::Operate() 
{
	Sleep(25 * number_of_threads);
	PulseEvent(start_threads_event);

	active_threads = number_of_threads;

	unsigned short thread_to_kill_number = 0;
	unsigned short thread_to_kill_index = 0;

	MarkerParameterData temp_marker_parameter_data;

	DWORD wait_result;

	while (active_threads)
	{
		wait_result = WaitForMultipleObjects(number_of_threads, threads_stopped_events, TRUE, INFINITE);

		if (wait_result >= WAIT_ABANDONED_0 && wait_result <= WAIT_ABANDONED_0 + number_of_threads - 1)
		{
			return HandleError("failed to wait for marker threads to stop!");
		}

		std::cout << "\nArray after all marker threads stopped:\n";
		PrintArray();

		thread_to_kill_index = GetThreadToKillIndex();

		std::cout << "Killing thread " << thread_to_kill_index << '\n';

		SetEvent(threads_parameter_data[thread_to_kill_index].exit_thread_event);
		wait_result = WaitForSingleObject(threads[thread_to_kill_index], INFINITE);
		if (wait_result == WAIT_FAILED)
		{
			return HandleError(std::string("failed to wait for marker thread of number " + thread_to_kill_number + '!'));
		}

		std::cout << "\nArray after marker thread of number " << thread_to_kill_number << " exited:\n";
		PrintArray();

		KillThread(thread_to_kill_index);

		PulseEvent(start_threads_event);
		Sleep(25 * active_threads);
	}

	system("pause");
	return 0;
}

DWORD ThreadManager::HandleError(std::string message) 
{
	DWORD error = GetLastError();
	std::cerr << message << '\n' << "error code: " << error;
	system("pause");
	return error;
}

unsigned short ThreadManager::GetThreadToKillIndex() 
{
	std::string temp_string;

	bool invalid_data_entered = true;

	unsigned short thread_number;

	while (true)
	{
		std::cout << "\nEnter the number of marker thread to kill:\n";
		std::getline(std::cin, temp_string);
		while (!CheckIfUnsignedShort(temp_string))
		{
			std::cout << "Enter the number of marker thread to kill:\n";
			std::getline(std::cin, temp_string);
		}

		thread_number = std::stoi(temp_string);

		for (unsigned short i = 0; i < number_of_threads; i++)
		{
			if (threads_parameter_data[i].array_data != nullptr && threads_parameter_data[i].thread_number == thread_number)
			{
				return i;
			}
		}

		std::cout << "Thread of this number does not exist!\n";
	}
}

void ThreadManager::KillThread(unsigned short thread_index) 
{
	CloseHandle(threads[thread_index]);
	CloseHandle(threads_parameter_data[thread_index].exit_thread_event);
	CloseHandle(threads_parameter_data[thread_index].thread_stopped_event);

	threads_parameter_data[thread_index].array_data = nullptr;

	active_threads--;
}

void ThreadManager::PrintArray() 
{
	for (size_t i = 0; i < array_size; i++) 
	{
		std::cout << array_data[i] << ' ';
	}
	std::cout << '\n';
}

ThreadManager::~ThreadManager() 
{
	delete[] threads_stopped_events;
	delete[] threads;
	delete[] threads_ids;
	delete[] threads_parameter_data;
}