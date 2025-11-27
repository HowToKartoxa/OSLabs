#ifndef _FILE_ACCESS_SERVER
#define _FILE_ACCESS_SERVER

#include <utils/employee_database.h>

#include <vector>

#include <windows.h>

class Server
{
	HANDLE* active_connections;

	std::vector<connection_data> active_connections_data;
	std::vector<STARTUPINFOA> clients_startup_info;
	std::vector<PROCESS_INFORMATION> clients_process_info;

	HANDLE output_log_mutex;

	EmployeeDB* database;

public:
	Server();
	~Server();
	DWORD Operate();
};

struct message
{
	char type;
	EmployeeDB::Employee data;

	message(char _type, EmployeeDB::Employee _data) : type(_type), data(_data) {}
};

enum message_types
{
	GET,
	GET_L,
	SET_L,
	RESP
};

DWORD WINAPI client_connection(LPVOID params);

struct connection_data
{
	unsigned short connection_number;
	EmployeeDB& database;

	connection_data(unsigned int _connection_number, EmployeeDB& _database) : 
		connection_number(_connection_number), 
		database(_database) 
	{}
};

#endif
