#include <utils/message_queue.h>
#include <iostream>

// file layout:

////////////////////////
//      capacity      // 
//        size        // 
//        front       //  
// entry slot 0       // 
// entry slot 1       // 
// entry slot 2       // 
// entry slot 3       // 
// entry slot 4       // 
//         .          //
//         .          //	
//         .          //
// entry slot cap - 3 // 
// entry slot cap - 2 // 
// entry slot cap - 1 // 
////////////////////////

MessageQueue::MessageQueue(std::string _file_name, LONG number_of_entries, bool own) :
	file_name(_file_name),
	is_owner(own)
{
	if (is_owner)
	{
		std::ofstream file(file_name, std::ios::binary);

		Info info(number_of_entries);
		file.write(reinterpret_cast<const char*>(&info), sizeof(Info));

		Message placeholder;
		for (LONG i = 0; i < number_of_entries; i++)
		{
			file.write(reinterpret_cast<const char*>(&placeholder), sizeof(Message));
		}

		enq_semaphore = CreateSemaphoreA(NULL, number_of_entries, number_of_entries, (file_name + "_MSG_Q_ENQ_SEM").c_str());
		if (enq_semaphore == NULL)
		{
			
		}
		deq_semaphore = CreateSemaphoreA(NULL, 0l, number_of_entries, (file_name + "_MSG_Q_DEQ_SEM").c_str());
		if (deq_semaphore == NULL)
		{

		}
		file_mutex = CreateMutexA(NULL, FALSE, (file_name + "_MSG_Q_MTX").c_str());
		if (file_mutex == NULL)
		{

		}
		file.close();
	}
	else
	{
		enq_semaphore = OpenSemaphoreA(SEMAPHORE_ALL_ACCESS, FALSE, (file_name + "_MSG_Q_ENQ_SEM").c_str());
		if (enq_semaphore == NULL)
		{

		}
		deq_semaphore = OpenSemaphoreA(SEMAPHORE_ALL_ACCESS, FALSE, (file_name + "_MSG_Q_DEQ_SEM").c_str());
		if (deq_semaphore == NULL)
		{

		}
		file_mutex = OpenMutexA(SYNCHRONIZE, FALSE, (file_name + "_MSG_Q_MTX").c_str());
		if (file_mutex == NULL)
		{

		}
	}
}

MessageQueue::~MessageQueue()
{
	if (is_owner)
	{
		CloseHandle(enq_semaphore);
		CloseHandle(deq_semaphore);
		CloseHandle(file_mutex);
	}
}

void MessageQueue::WEnqueue(MessageQueue::Message message, DWORD wait_for)
{
	WaitForSingleObject(enq_semaphore, wait_for);
	WaitForSingleObject(file_mutex, INFINITE);

	std::fstream file(file_name, std::ios::binary | std::ios::in | std::ios::out);

	Info info;
	file.read(reinterpret_cast<char*>(&info), sizeof(Info));

	file.seekp(sizeof(Info) + sizeof(Message) * ((info.front + info.size) % info.capacity), std::ios::beg);
	file.write(reinterpret_cast<const char*>(&message), sizeof(Message));

	info.size++;
	file.seekp(0, std::ios::beg);
	file.write(reinterpret_cast<const char*>(&info), sizeof(Info));

	file.close();

	ReleaseMutex(file_mutex);
	ReleaseSemaphore(deq_semaphore, 1, NULL);
}

MessageQueue::Message MessageQueue::WDequeue(DWORD wait_for)
{
	WaitForSingleObject(deq_semaphore, wait_for);
	WaitForSingleObject(file_mutex, INFINITE);

	std::fstream file(file_name, std::ios::binary | std::ios::in | std::ios::out);

	Info info;
	file.read(reinterpret_cast<char*>(&info), sizeof(Info));

	Message res;
	file.seekg(sizeof(Info) + sizeof(Message) * info.front, std::ios::beg);
	file.read(reinterpret_cast<char*>(&res), sizeof(Message));

	info.front = (info.front + 1) % info.capacity;
	info.size--;
	file.seekp(0, std::ios::beg);
	file.write(reinterpret_cast<const char*>(&info), sizeof(Info));

	file.close();
	ReleaseMutex(file_mutex);
	ReleaseSemaphore(enq_semaphore, 1, NULL);
	return res;
}