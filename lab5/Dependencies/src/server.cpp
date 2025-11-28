#include <base/server.h>
#include <utils/input_parsing.h>
#include <base/message.h>

#include <iostream>
#include <string>

#include <windows.h>

#undef max;

Server::Server() : 
	active_connections(nullptr), 
	active_connections_data(),
	clients_startup_info(),
	clients_process_info(),
	output_log_mutex(nullptr),
	database(nullptr) {}

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

	output_log_mutex = CreateMutexA(NULL, FALSE, "SERVER_OUTPUT_LOG_MUTEX");
	if (output_log_mutex == INVALID_HANDLE_VALUE)
	{
		//
	}

	std::cout << "Initializing clients ...\n";

	unsigned short number_of_clients = Query<unsigned short>("Enter number of client processes:");
	active_connections = new HANDLE[number_of_clients];

	HANDLE temp_thread_handle;
	std::string temp_command_line = "Client.exe ";
	DWORD wait_result;

	for (unsigned short i = 0; i < number_of_clients; i++)
	{
		active_connections_data.emplace_back(i, database);
		temp_thread_handle = CreateThread(NULL, NULL, client_connection, &active_connections_data[i], NULL, NULL);
		if (temp_thread_handle == INVALID_HANDLE_VALUE)
		{
			//
		}
		active_connections[i] = temp_thread_handle;

		clients_startup_info.emplace_back();
		clients_process_info.emplace_back();
		ZeroMemory(&clients_startup_info[i], sizeof(STARTUPINFOA));
		clients_startup_info[i].cb = sizeof(STARTUPINFOA);
		temp_command_line += std::to_string(i);
		if (!CreateProcessA
		(
			NULL,
			const_cast<char*>(temp_command_line.c_str()),
			NULL,
			NULL,
			FALSE,
			CREATE_NEW_CONSOLE,
			NULL,
			NULL,
			&clients_startup_info[i],
			&clients_process_info[i]
		))
		{
			//
		}

		wait_result = WaitForSingleObject(output_log_mutex, INFINITE);
		if (wait_result != WAIT_OBJECT_0)
		{
			//
		}
		std::cout << "CLIENT PROCESS [" << i << "] initialized!\n";
		ReleaseMutex(output_log_mutex);
	}
}

DWORD WINAPI client_connection(LPVOID params)
{
	HANDLE output_log_mutex = OpenMutexA(SYNCHRONIZE, FALSE, "SERVER_OUTPUT_LOG_MUTEX");
	if (output_log_mutex == INVALID_HANDLE_VALUE)
	{
		//
	}

	connection_data info = *reinterpret_cast<connection_data*>(params);

	DWORD wait_result = WaitForSingleObject(output_log_mutex, INFINITE);
	if (wait_result != WAIT_OBJECT_0)
	{
		//
	}
	std::cout << "CONNECTION [" << info.connection_number << "] initialized!\n";
	ReleaseMutex(output_log_mutex);

	HANDLE pipe = CreateNamedPipeA
	(
		(std::string("\\\\.\\pipe\\client_pipe_") + std::to_string(info.connection_number)).c_str(),
		PIPE_ACCESS_DUPLEX,
		PIPE_TYPE_MESSAGE | PIPE_WAIT,
		1,
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

	wait_result = WaitForSingleObject(output_log_mutex, INFINITE);
	if (wait_result != WAIT_OBJECT_0)
	{
		//
	}
	std::cout << "CONNECTED TO CLIENT [" << info.connection_number << "]!\n";
	ReleaseMutex(output_log_mutex);

	message buffer(0, EmployeeDB::Employee(0, "", 0));
	DWORD bytes_read;
	DWORD bytes_written;
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
				wait_result = WaitForSingleObject(output_log_mutex, INFINITE);
				if (wait_result != WAIT_OBJECT_0)
				{
					//
				}
				std::cout << "{GET} " << buffer.data.id << " FROM [" << info.connection_number << "]\n";
				ReleaseMutex(output_log_mutex);

				if (info.database.WGet(buffer.data.id, buffer.data))
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
				wait_result = WaitForSingleObject(output_log_mutex, INFINITE);
				if (wait_result != WAIT_OBJECT_0)
				{
					//
				}
				std::cout << "{GET AND LOCK} " << buffer.data.id << " FROM [" << info.connection_number << "]\n";
				ReleaseMutex(output_log_mutex);

				if (info.database.WGetAndLock(buffer.data.id, buffer.data, locked_at))
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
				wait_result = WaitForSingleObject(output_log_mutex, INFINITE);
				if (wait_result != WAIT_OBJECT_0)
				{
					//
				}
				std::cout << "{SET AND UNLOCK} " << buffer.data.id << " FROM [" << info.connection_number << "]\n";
				ReleaseMutex(output_log_mutex);

				if (info.database.WSetAndUnlock(buffer.data.id, buffer.data, locked_at))
				{
					//
				}
				break;
			}
		}
	}
}