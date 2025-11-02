#include <utils/message_queue.h>

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


MessageQueue::MessageQueue(std::string _file_name, long number_of_entries, bool create_new) :
	file_name(_file_name)
{
	std::fstream file(file_name);
	if (create_new)
	{
		Info info(number_of_entries);
		file.write(reinterpret_cast<const char*>(&info), sizeof(Info));

		Message placeholder;
		for (long i = 0; i < number_of_entries; i++)
		{
			file.write(reinterpret_cast<const char*>(&placeholder), sizeof(Message));
		}

		enq_semaphore = CreateSemaphoreA(NULL, 0l, number_of_entries, (file_name + "_MSG_Q_ENQ_SEM").c_str());
		if (enq_semaphore == NULL)
		{
			
		}
		deq_semaphore = CreateSemaphoreA(NULL, 0l, number_of_entries, (file_name + "_MSG_Q_DEQ_SEM").c_str());
		if (deq_semaphore == NULL)
		{

		}
	}
	else
	{
		Info info;
		file.read(reinterpret_cast<char*>(&info), sizeof(Info));
		
		enq_semaphore = CreateSemaphoreA(NULL, info.size, info.capacity, (file_name + "_MSG_Q_ENQ_SEM").c_str());
		if (enq_semaphore == NULL)
		{

		}
		deq_semaphore = CreateSemaphoreA(NULL, info.size, info.capacity, (file_name + "_MSG_Q_DEQ_SEM").c_str());
		if (deq_semaphore == NULL)
		{

		}
	}
	file_mutex = CreateMutexA(NULL, FALSE, (file_name + "_MSG_Q_MTX").c_str());
	if (file_mutex == NULL)
	{

	}
}

MessageQueue::~MessageQueue()
{
	std::remove(file_name.c_str());
	CloseHandle(enq_semaphore);
	CloseHandle(deq_semaphore);
}

void MessageQueue::Enqueue(MessageQueue::Message message, DWORD wait_for)
{
	WaitForSingleObject(enq_semaphore, wait_for);
	WaitForSingleObject(file_mutex, INFINITE);
	std::fstream file(file_name);
	
	Info info;
	file.read(reinterpret_cast<char*>(&info), sizeof(Info));

	file.seekp(sizeof(info) + sizeof(Message) * ((info.front + info.size) % info.capacity));
	file.write(reinterpret_cast<const char*>(&message), sizeof(Message));

	info.size++;
	file.seekp(0);
	file.write(reinterpret_cast<const char*>(&info), sizeof(Info));

	file.close();
	ReleaseSemaphore(deq_semaphore, 1, NULL);
	ReleaseMutex(file_mutex);
}

MessageQueue::Message MessageQueue::Dequeue(DWORD wait_for)
{
	WaitForSingleObject(deq_semaphore, wait_for);
	WaitForSingleObject(file_mutex, INFINITE);
	std::fstream file(file_name);

	Info info;
	file.read(reinterpret_cast<char*>(&info), sizeof(Info));

	Message res;
	file.seekg(sizeof(Info) + sizeof(Message) * info.front);
	file.read(reinterpret_cast<char*>(&res), sizeof(Message));

	info.front = (info.front + 1) % info.capacity;
	info.size--;
	file.write(reinterpret_cast<const char*>(&info), sizeof(Info));

	file.close();
	ReleaseSemaphore(enq_semaphore, 1, NULL);
	ReleaseMutex(file_mutex);
}