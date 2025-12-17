#ifndef _FILE_ACCESS_CLIENT_BOOST
#define _FILE_ACCESS_CLIENT_BOOST

#include <boost/interprocess/ipc/message_queue.hpp>

class Client
{
	boost::interprocess::message_queue in;
	boost::interprocess::message_queue out;
	unsigned short client_number;

public:
	Client(char* mq_name, unsigned short _client_number);
	~Client();
	void Operate();

};

#endif