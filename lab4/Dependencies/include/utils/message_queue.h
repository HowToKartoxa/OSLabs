#ifndef MESSAGE_QUEUE_
#define MESSAGE_QUEUE_

#include <fstream>
#include <string>

#define USE_WINAPI

#if defined(USE_WINAPI)

#include <windows.h>

#elif defined(USE_BOOST)

#include <boost/thread.h>

#endif

class MessageQueue
{
private:
	std::string file_name;

#if defined(USE_WINAPI)

	HANDLE enq_semaphore;
	HANDLE deq_semaphore;

	HANDLE file_mutex;

#elif defined(USE_BOOST)

#endif

public:
	struct Info
	{
		long capacity;
		long size;
		long front;

		Info() : capacity(0l), size(0l), front(0l) {}
		Info(long _capacity) : capacity(_capacity), size(0l), front(0l) {}
	};
	struct Message
	{
		char data[21];

		Message() : data("\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0") {}
		Message(std::string str) : data("\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0")
		{
			for (size_t i = 0; i < 20 && i < str.length(); i++)
			{
				data[i] = str[i];
			}
		}
	};

	MessageQueue(std::string file_name, long number_of_entries, bool create_new = true);
	~MessageQueue();

	void Enqueue(Message message, DWORD wait_for = INFINITE);
	Message Dequeue(DWORD wait_for = INFINITE);
};

#endif