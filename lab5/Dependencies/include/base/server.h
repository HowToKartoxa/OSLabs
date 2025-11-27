#ifndef _FILE_ACCESS_SERVER
#define _FILE_ACCESS_SERVER

#include <utils/employee_database.h>

#include <vector>

#include <windows.h>

class Server
{
	std::vector<HANDLE> active_connections;
	std::vector<connection_data> active_connections_data;
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
	std::string database_name;
	std::vector<SRWLOCK>& locks;

	connection_data(unsigned int _connection_number, std::string _database_name, std::vector<SRWLOCK>& _locks) : 
		locks(_locks), 
		connection_number(_connection_number), 
		database_name(_database_name) {}
};

#endif
