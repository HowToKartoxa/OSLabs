#ifndef MESSAGE_QUEUE_BOOST_
#define MESSAGE_QUEUE_BOOST_

#include <fstream>
#include <string>

#include <boost/thread.hpp>
#include <boost/interprocess/sync/named_mutex.hpp>
#include <boost/interprocess/sync/named_semaphore.hpp>
#include <boost/date_time.hpp>

class MessageQueue
{
private:
	std::string file_name;
	bool is_owner;

	boost::interprocess::named_semaphore* enq_semaphore;
	boost::interprocess::named_semaphore* deq_semaphore;

	boost::interprocess::named_mutex* file_mutex;

public:
	struct Info
	{
		unsigned int capacity;
		unsigned int size;
		unsigned int front;

		Info() : capacity(0l), size(0l), front(0l) {}
		Info(long _capacity) : capacity(_capacity), size(0l), front(0l) {}
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

	MessageQueue(std::string file_name, unsigned int number_of_entries, bool own = true);
	~MessageQueue();


	bool WEnqueue(Message message);
	bool WDequeue(Message& destination);
	bool WEnqueue(Message message, boost::posix_time::ptime wait_for);
	bool WDequeue(Message& destination, boost::posix_time::ptime wait_for);
};

#endif
