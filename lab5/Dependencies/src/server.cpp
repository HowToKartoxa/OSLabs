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
	database(nullptr) 
{
	output_log_mutex = CreateMutexA(NULL, FALSE, "SERVER_OUTPUT_LOG_MUTEX");
	if (output_log_mutex == NULL)
	{
		//
	}
}

Server::~Server()
{
	delete database;
}

unsigned short Server::new_client_connection()
{
	unsigned short index = active_connections_data.size();

	active_connections_data.emplace_back(index, *database);
	HANDLE temp_thread_handle = CreateThread(NULL, NULL, client_connection, &active_connections_data[index], NULL, NULL);
	if (temp_thread_handle == INVALID_HANDLE_VALUE)
	{
		//
	}
	active_connections[index] = temp_thread_handle;

	return index;
}

unsigned short Server::new_client_process()
{
	unsigned short index = clients_startup_info.size();

	clients_startup_info.emplace_back();
	clients_process_info.emplace_back();
	ZeroMemory(&clients_startup_info[index], sizeof(STARTUPINFOA));
	clients_startup_info[index].cb = sizeof(STARTUPINFOA);

	if (!CreateProcessA
	(
		NULL,
		const_cast<char*>
		(
			(
				std::string("Client.exe ")
				+ std::to_string(index)
				+ std::string(" \\\\.\\pipe\\client_pipe_")
				+ std::to_string(index)
			).c_str()
		),
		NULL,
		NULL,
		FALSE,
		CREATE_NEW_CONSOLE,
		NULL,
		NULL,
		&clients_startup_info[index],
		&clients_process_info[index]
	))
	{
		//
	}

	return index;
}

void Server::log(std::string message)
{
	DWORD wait_result = WaitForSingleObject(output_log_mutex, INFINITE);
	if (wait_result != WAIT_OBJECT_0)
	{
		//
	}
	std::cout << message << '\n';
	ReleaseMutex(output_log_mutex);
}

DWORD Server::Operate()
{
	std::string file_name = QueryFileName("Enter binary file name:", "");

	unsigned int number_of_employees = Query<unsigned int>("Enter the number of employees in the file:");

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

	std::cout << "Initializing clients ...\n";

	unsigned short number_of_clients = Query<unsigned short>("Enter number of client processes:");

	std::string temp_command_line;
	active_connections = new HANDLE[number_of_clients];
	unsigned short index;

	for (unsigned short i = 0; i < number_of_clients; i++)
	{
		new_client_connection();
		index = new_client_process();

		log(std::string("CLIENT PROCESS [" + std::to_string(index) + "] created!"));
	}
	DWORD wait_result = WaitForMultipleObjects(number_of_clients, active_connections, TRUE, INFINITE);
	if (wait_result < WAIT_OBJECT_0 || wait_result > WAIT_OBJECT_0 + number_of_clients)
	{
		//
	}
	return 0ul;
}

DWORD WINAPI client_connection(LPVOID params)
{
	HANDLE output_log_mutex = OpenMutexA(SYNCHRONIZE, FALSE, "SERVER_OUTPUT_LOG_MUTEX");
	if (output_log_mutex == INVALID_HANDLE_VALUE)
	{
		//
	}

	connection_data info = *reinterpret_cast<connection_data*>(params);

	if (WaitForSingleObject(output_log_mutex, INFINITE) != WAIT_OBJECT_0)
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

	if (WaitForSingleObject(output_log_mutex, INFINITE) != WAIT_OBJECT_0)
	{
		//
	}
	std::cout << "CONNECTED TO CLIENT [" << info.connection_number << "]!\n";
	ReleaseMutex(output_log_mutex);

	message buffer(message_types::FOUND, EmployeeDB::Employee(0, "", 0));
	DWORD bytes_read;
	DWORD bytes_written;
	size_t locked_at;

	bool operational = true;

	while (operational)
	{
		if (!ReadFile(pipe, reinterpret_cast<void*>(&buffer), sizeof(message), &bytes_read, NULL))
		{
			//
		}
		switch (buffer.type)
		{
			case message_types::GET_SHARED: 
			{
				if (WaitForSingleObject(output_log_mutex, INFINITE) != WAIT_OBJECT_0)
				{
					//
				}
				std::cout << "{GET SHARED} " << buffer.data.id << " FROM [" << info.connection_number << "]\n";
				ReleaseMutex(output_log_mutex);

				if (info.database.WGetShared(buffer.data.id, buffer.data, locked_at))
				{
					buffer.type = message_types::NOT_FOUND;
				}
				else
				{
					buffer.type = message_types::FOUND;
				}
				
				if (WriteFile(pipe, reinterpret_cast<void*>(&buffer), sizeof(message), &bytes_written, NULL))
				{
					//
				}
				break;
			}
			case message_types::GET_EXCLUSIVE:
			{
				if (WaitForSingleObject(output_log_mutex, INFINITE) != WAIT_OBJECT_0)
				{
					//
				}
				std::cout << "{GET EXCLUSIVE} " << buffer.data.id << " FROM [" << info.connection_number << "]\n";
				ReleaseMutex(output_log_mutex);

				if (info.database.WGetExclusive(buffer.data.id, buffer.data, locked_at))
				{
					buffer.type = message_types::NOT_FOUND;
				}
				else
				{
					buffer.type = message_types::FOUND;
				}

				if (WriteFile(pipe, reinterpret_cast<void*>(&buffer), sizeof(message), &bytes_written, NULL))
				{
					//
				}
				break;
			}
			case message_types::SET: 
			{
				if (WaitForSingleObject(output_log_mutex, INFINITE) != WAIT_OBJECT_0)
				{
					//
				}
				std::cout << "{SET} " << locked_at << " FROM [" << info.connection_number << "]\n";
				ReleaseMutex(output_log_mutex);

				info.database.Set(buffer.data, locked_at);

				buffer.type = message_types::FOUND;
				if (WriteFile(pipe, reinterpret_cast<void*>(&buffer), sizeof(message), &bytes_written, NULL))
				{
					//
				}
				break;
			}
			case message_types::UNLOCK_SHARED: 
			{
				if (WaitForSingleObject(output_log_mutex, INFINITE) != WAIT_OBJECT_0)
				{
					//
				}
				std::cout << "{UNLOCK SHARED} " << buffer.data.id << " FROM [" << info.connection_number << "]\n";
				ReleaseMutex(output_log_mutex);

				info.database.UnlockShared(locked_at);

				buffer.type = message_types::FOUND;
				if (WriteFile(pipe, reinterpret_cast<void*>(&buffer), sizeof(message), &bytes_written, NULL))
				{
					//
				}
				break;
			}
			case message_types::UNLOCK_EXCLUSIVE: 
			{
				if (WaitForSingleObject(output_log_mutex, INFINITE) != WAIT_OBJECT_0)
				{
					//
				}
				std::cout << "{UNLOCK EXCLUSIVE} " << buffer.data.id << " FROM [" << info.connection_number << "]\n";
				ReleaseMutex(output_log_mutex);

				info.database.UnlockExclusive(locked_at);

				buffer.type = message_types::FOUND;
				if (WriteFile(pipe, reinterpret_cast<void*>(&buffer), sizeof(message), &bytes_written, NULL))
				{
					//
				}
				break;
			}
			case message_types::SHUTDOWN: 
			{
				if (WaitForSingleObject(output_log_mutex, INFINITE) != WAIT_OBJECT_0)
				{
					//
				}

				buffer.type = message_types::FOUND;
				if (WriteFile(pipe, reinterpret_cast<void*>(&buffer), sizeof(message), &bytes_written, NULL))
				{
					//
				}

				std::cout << "CONNECTION[" << info.connection_number << "] terminated!\n";
				ReleaseMutex(output_log_mutex);

				operational = false;
				break;
			}
		}
	}

	return 0ul;
}