#ifndef _FILE_ACCESS_SERVER
#define _FILE_ACCESS_SERVER

#include <windows.h>
#include <utils/employee_database.h>

class Server
{
	unsigned short client_count;
	HANDLE* active_connections;
	EmployeeDB database;

public:
	Server();
	~Server();
	DWORD Operate();
};

#endif
