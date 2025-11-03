#ifndef MESSAGE_QUEUE_
#define MESSAGE_QUEUE_

#include <fstream>
#include <string>

#include <windows.h>

class MessageQueue
{
private:
	std::string file_name;
	bool is_owner;

	HANDLE enq_semaphore;
	HANDLE deq_semaphore;

	HANDLE file_mutex;

public:
	struct Info
	{
		LONG capacity;
		LONG size;
		LONG front;

		Info() : capacity(0l), size(0l), front(0l) {}
		Info(LONG _capacity) : capacity(_capacity), size(0l), front(0l) {}
	};
	struct Message
	{
		char data[20];
		unsigned short author;

		Message() : data("\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"), author(0) {}
		Message(std::string str, unsigned short _author) : data("\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"), author(_author)
		{
			for (size_t i = 0; i < 19 && i < str.length(); i++)
			{
				data[i] = str[i];
			}
		}

		Message& operator=(const Message& src)
		{
			for (char i = 0; i < 20; i++)
			{
				data[i] = src.data[i];
			}
			author = src.author;
			return *this;
		}
	};

	MessageQueue(std::string file_name, LONG number_of_entries, bool own = true);
	~MessageQueue();

	bool WEnqueue(Message message, DWORD wait_for = INFINITE);
	bool WDequeue(Message& destination, DWORD wait_for = INFINITE);
};

#endif