#include <utils/thread_manager_boost.h>
#include <utils/marker_procedures.h>
#include <utils/input_parsing.h>

#include <boost/thread.hpp>

#include <iostream>

ThreadManagerBoost::ThreadManagerBoost(int* _array_data, size_t _array_size, unsigned short _thread_num) :  
	output_mutex(),
	array_mutex(),
	start_threads_event(),
	threads_stopped_events(_thread_num)
{
	array_data = _array_data;
	array_size = _array_size;
	number_of_threads = _thread_num;
	active_threads = 0;

	response_events = new MultiEvent* [number_of_threads];

	for (unsigned short i = 0; i < number_of_threads; i++) 
	{
		response_events[i] = new MultiEvent(2);
	}

	try 
	{
		InitializeThreads();
	}
	catch (...) 
	{

	}
}

void ThreadManagerBoost::InitializeThreads() 
{
	threads = new boost::thread* [number_of_threads];

	for (unsigned short i = 0; i < number_of_threads; i++) 
	{
		threads[i] = new boost::thread
		(
			marker_boost,
			array_data,
			array_size,
			i + 1,
			&output_mutex,
			&array_mutex,
			&start_threads_event,
			&threads_stopped_events,
			response_events
		);
	}
}

void ThreadManagerBoost::Operate() 
{
	boost::this_thread::sleep_for(boost::chrono::milliseconds(25 * number_of_threads));
	start_threads_event.Set();

	active_threads = number_of_threads;

	unsigned short thread_to_kill_number = 0;

	while (active_threads) 
	{
		threads_stopped_events.WaitAll();
		threads_stopped_events.ResetAll();

		std::cout << "\nArray after all marker threads stopped:\n";
		PrintArray();

		thread_to_kill_number = GetThreadToKillNumber();

		std::cout << "Killing thread " << thread_to_kill_number << '\n';

		response_events[thread_to_kill_number - 1]->Set(0);

		threads[thread_to_kill_number - 1]->join();

		std::cout << "\nArray after marker thread of number " << thread_to_kill_number << " exited:\n";
		PrintArray();

		KillThread(thread_to_kill_number - 1);

		for (unsigned short i = 0; i < active_threads; i++) 
		{
			if (threads[i] != nullptr) 
			{
				response_events[i]->Set(1);
			}
		}

		active_threads--;
		boost::this_thread::sleep_for(boost::chrono::milliseconds(50 * active_threads));
	}

	system("pause");
}

unsigned short ThreadManagerBoost::GetThreadToKillNumber() 
{
	std::string temp_string;

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

		if (threads[thread_number - 1] != nullptr) 
		{
			return thread_number;
		}

		std::cout << "Thread of this number does not exist!\n";
	}
}

void ThreadManagerBoost::KillThread(unsigned short thread_index) 
{
	threads_stopped_events.Deactivate(thread_index);
	delete threads[thread_index];
}

void ThreadManagerBoost::PrintArray()
{
	for (size_t i = 0; i < array_size; i++)
	{
		std::cout << array_data[i] << ' ';
	}
	std::cout << '\n';
}

ThreadManagerBoost::~ThreadManagerBoost() 
{
	delete[] array_data;
	delete[] threads;
	for (unsigned int i = 0; i < number_of_threads; i++) 
	{
		delete response_events[i];
	}
}