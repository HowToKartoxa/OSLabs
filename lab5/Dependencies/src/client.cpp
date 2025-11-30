#include <base/client.h>
#include <base/message.h>
#include <utils/input_parsing.h>

#include <iostream>
#include <string>

Client::Client(char* pipe_name, unsigned short _client_number) : client_number(_client_number)
{
	pipe = CreateFileA
	(
		pipe_name,
		GENERIC_READ | GENERIC_WRITE,
		NULL,
		NULL,
		OPEN_EXISTING,
		NULL,
		NULL
	);
	if (pipe == INVALID_HANDLE_VALUE)
	{
		//
	}
}

Client::~Client()
{
	CloseHandle(pipe);
}

DWORD Client::Operate()
{
	std::cout << "Client [" << client_number << "]\n";
	std::string temp_string = "";
	message buffer(message_types::RESP, EmployeeDB::Employee(0, "", 0));
	DWORD bytes_read;
	DWORD bytes_written;

	while (true)
	{
		std::cout << "Enter 'g' to get table entry by id, 's' to get and change table entry by id, enter anything else to disconnect from the server:\n";

		std::getline(std::cin, temp_string);
		if (temp_string.empty())
		{
			break;
		}
		else
		{
			if (temp_string[0] == 'g')
			{
				buffer.data.id = Query<unsigned int>("Enter id of the employee:");
				buffer.type = message_types::GET_SHARED;

				if (!WriteFile(pipe, reinterpret_cast<void*>(&buffer), sizeof(message), &bytes_written, NULL))
				{
					//
				}
				if (!ReadFile(pipe, reinterpret_cast<void*>(&buffer), sizeof(message), &bytes_read, NULL))
				{
					//
				}

				std::cout << buffer.data.id << '\n' << buffer.data.name << '\n' << buffer.data.hours << '\n';
				buffer.type = message_types::UNLOCK_SHARED;
				std::cout << "Press any key to stop access to the table entry:\n";
				std::cin.get();

				if (!WriteFile(pipe, reinterpret_cast<void*>(&buffer), sizeof(message), &bytes_written, NULL))
				{
					//
				}
				if (!ReadFile(pipe, reinterpret_cast<void*>(&buffer), sizeof(message), &bytes_read, NULL))
				{
					//
				}
			}
			else if (temp_string[0] == 's')
			{
				buffer.data.id = Query<unsigned int>("Enter id of the employee:");
				buffer.type = message_types::GET_EXCLUSIVE;

				if (!WriteFile(pipe, reinterpret_cast<void*>(&buffer), sizeof(message), &bytes_written, NULL))
				{
					//
				}
				if (!ReadFile(pipe, reinterpret_cast<void*>(&buffer), sizeof(message), &bytes_read, NULL))
				{
					//
				}
				std::cout << buffer.data.id << '\n' << buffer.data.name << '\n' << buffer.data.hours << '\n';

				buffer.data.id = Query<unsigned int>("Enter new id:");
				temp_string = Query<std::string>("Enter new name:");
				strcpy_s(buffer.data.name, temp_string.c_str());
				buffer.data.hours = Query<double>("Enter new hours:");
				buffer.type = message_types::SET;
				std::cout << "Press any key to send new data:\n";
				std::cin.get();

				if (!WriteFile(pipe, reinterpret_cast<void*>(&buffer), sizeof(message), &bytes_written, NULL))
				{
					//
				}
				if (!ReadFile(pipe, reinterpret_cast<void*>(&buffer), sizeof(message), &bytes_read, NULL))
				{
					//
				}

				buffer.type = message_types::UNLOCK_EXCLUSIVE;
				std::cout << "Press any key to stop access to the table entry:\n";
				std::cin.get();

				if (!WriteFile(pipe, reinterpret_cast<void*>(&buffer), sizeof(message), &bytes_written, NULL))
				{
					//
				}
				if (!ReadFile(pipe, reinterpret_cast<void*>(&buffer), sizeof(message), &bytes_read, NULL))
				{
					//
				}
			}
			else 
			{
				break; 
			}
		}
	}
	return 0ul;
}
