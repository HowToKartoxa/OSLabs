#ifndef _FILE_ACCESS_SERVER_BOOST
#define _FILE_ACCESS_SERVER_BOOST

#include <utils/input_parsing.h>

#include <boost/mysql.hpp>
#include <boost/process/v2/process.hpp>
#include <boost/thread.hpp>

class Server
{
public:
	Server();
	~Server();

	void Operate();

};

#endif