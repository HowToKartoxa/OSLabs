#ifndef _FILE_ACCESS_SERVER_BOOST
#define _FILE_ACCESS_SERVER_BOOST

#include <utils/employee_database.h>

#include <vector>
#include <string>

#include <boost/thread.hpp>
#include <boost/asio/io_context.hpp>

class Server
{
	boost::mutex output_log_mutex;
	boost::thread_group active_connections;
	std::vector<boost::asio::io_context*> client_processes_io_contexts;

	EmployeeDB* database;

	unsigned short new_client_connection();
	unsigned short new_client_process();
	void log(std::string message);

public:
	Server();
	~Server();

	void Operate();
};

void client_connection(unsigned short connection_number, boost::mutex* output_log_mutex, EmployeeDB* database);

#endif