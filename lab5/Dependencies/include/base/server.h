#ifndef _FILE_ACCESS_SERVER
#define _FILE_ACCESS_SERVER

#include <utils/employee_database.h>

#include <vector>

#include <windows.h>

class Server
{
	std::vector<HANDLE> active_connections;
	EmployeeDB* database;

	void query_input_data();

public:
	Server();
	~Server();
	DWORD Operate();
};

#endif
