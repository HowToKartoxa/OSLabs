#include <base/server.h>
#include <utils/input_parsing.h>

#include <iostream>
#include <string>

#include <windows.h>

#undef max;

Server::Server() : active_connections(), active_connections_data(), database(nullptr) {}

Server::~Server()
{
	delete database;
}

DWORD Server::Operate()
{
	std::string file_name = QueryFileName("Enter binary file name:", "");

	unsigned int number_of_employees = Query<unsigned int>("Enter the number of employees in the file");

	std::vector<EmployeeDB::Employee> data(number_of_employees);
	std::vector<SRWLOCK> locks(number_of_employees);

	std::string temp_string;
	for (unsigned int i = 0; i < number_of_employees; i++)
	{
		data[i].id = Query<unsigned int>("Enter employee ID:");
		temp_string = Query<std::string>("Enter employee name:");
		strcpy_s(data[i].name, temp_string.c_str());
		data[i].hours = Query<double>("Enter employee hours:");
		InitializeSRWLock(&locks[i]);
	}

	database = new EmployeeDB(file_name, data, locks);

	unsigned short number_of_clients = Query<unsigned short>("Enter number of client processes:");

	HANDLE temp_thread_handle;
	for (unsigned short i = 0; i < number_of_clients; i++)
	{
		active_connections_data.emplace_back(i, file_name, )
		temp_thread_handle = CreateThread(NULL, NULL, client_connection, &)
	}
}

DWORD WINAPI client_connection(LPVOID params)
{
	connection_data data = *reinterpret_cast<connection_data*>(params);

	HANDLE pipe = CreateNamedPipeA
	(
		(std::string("CLIENT_PIPE_") + std::to_string(data.connection_number)).c_str(),
		PIPE_ACCESS_DUPLEX,
		PIPE_TYPE_MESSAGE,
		2,
		sizeof(message),
		sizeof(message),
		INFINITE,
		NULL
	);
	if (pipe == INVALID_HANDLE_VALUE)
	{
		//
	}
	if (!ConnectNamedPipe(pipe, NULL))
	{
		//
	}

	message buffer(0, EmployeeDB::Employee(0, "", 0));
	DWORD bytes_read;
	DWORD bytes_written;
	EmployeeDB database(data.database_name, data.locks);
	size_t locked_at;

	while (true)
	{
		if (!ReadFile(pipe, reinterpret_cast<void*>(&buffer), sizeof(message), &bytes_read, NULL))
		{

		}
		switch (buffer.type)
		{
			case message_types::GET: 
			{
				if (database.WGet(buffer.data.id, buffer.data))
				{
					//
				}
				if (WriteFile(pipe, reinterpret_cast<void*>(&buffer), sizeof(message), &bytes_written, NULL))
				{
					//
				}
				break;
			}
			case message_types::GET_L: 
			{
				if (database.WGetAndLock(buffer.data.id, buffer.data, locked_at))
				{
					//
				}
				if (WriteFile(pipe, reinterpret_cast<void*>(&buffer), sizeof(message), &bytes_written, NULL))
				{
					//
				}
				break;
			}
			case message_types::SET_L: 
			{
				if (database.WSetAndUnlock(buffer.data.id, buffer.data, locked_at))
				{
					//
				}
				break;
			}
		}
	}
}