#ifndef _FILE_ACCESS_SERVER
#define _FILE_ACCESS_SERVER

#include <utils/employee_database.h>

#include <vector>

#include <windows.h>

struct connection_data
{
	unsigned short connection_number;
	EmployeeDB& database;

	connection_data(unsigned int _connection_number, EmployeeDB& _database) :
		connection_number(_connection_number),
		database(_database)
	{}
};

class Server
{
	HANDLE* active_connections;

	std::vector<connection_data> active_connections_data;
	std::vector<STARTUPINFOA> clients_startup_info;
	std::vector<PROCESS_INFORMATION> clients_process_info;

	HANDLE output_log_mutex;

	EmployeeDB* database;

	DWORD error_code;
	std::string error_message;

public:
	Server();
	~Server();
	DWORD Operate();

	bool Ok();
	DWORD GetErrorCode();
	std::string GetErrorMessage();


private:
	unsigned short new_client_connection();
	unsigned short new_client_process();
	void log(std::string message);
	void raise(DWORD code, std::string message);
};

DWORD WINAPI client_connection(LPVOID params);

#endif
