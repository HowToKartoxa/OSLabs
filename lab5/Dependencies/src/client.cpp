#include <base/client.h>
#include <base/message.h>
#include <utils/input_parsing.h>
#include <utils/employee.h>

#include <iostream>
#include <string>

void Client::raise(DWORD code, std::string message)
{
	error_code = code;
	error_message = message;
}

bool Client::Ok()
{
	return error_code == NULL;
}

DWORD Client::GetErrorCode()
{
	return error_code;
}

std::string Client::GetErrorMessage()
{
	return error_message;
}

Client::Client(char* pipe_name, unsigned short _client_number) : client_number(_client_number), error_code(NULL), error_message("")
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
		raise(GetLastError(), "Failed to create named pipe");
	}
}

Client::~Client()
{
	CloseHandle(pipe);
}

DWORD Client::Operate()
{
	if (!Ok())
	{
		return error_code;
	}

	std::cout << "Client [" << client_number << "]\n";
	std::string temp_string = "";
	size_t temp_id;
	message buffer(message_types::FOUND, 0, Employee(0, "", 0));
	DWORD bytes_read;
	DWORD bytes_written;

	while (true)
	{
		temp_string = Query<std::string>("Enter 'g' to get table entry by id, 's' to get and change table entry by id, enter anything else to disconnect from the server:");

		if (temp_string.empty())
		{
			buffer.type = message_types::SHUTDOWN;
			if (!WriteFile(pipe, reinterpret_cast<void*>(&buffer), sizeof(message), &bytes_written, NULL))
			{
				raise(GetLastError(), std::string("Failed to write to named pipe when sending a SHUTDOWN message, bytes written: ") += std::to_string(bytes_written));
				return error_code;
			}
			if (!ReadFile(pipe, reinterpret_cast<void*>(&buffer), sizeof(message), &bytes_read, NULL))
			{
				raise(GetLastError(), std::string("Failed to get a response to SHUTDOWN message from server, bytes read: ") += std::to_string(bytes_read));
				return error_code;
			}
			return 0ul;
		}

		if (temp_string[0] == 'g')
		{
			buffer.id = Query<size_t>("Enter id of the employee:");
			buffer.type = message_types::GET_SHARED;

			if (!WriteFile(pipe, reinterpret_cast<void*>(&buffer), sizeof(message), &bytes_written, NULL))
			{
				raise(GetLastError(), std::string("Failed to write to named pipe when sending a GET_SHARED message, bytes written: ") += std::to_string(bytes_written));
				return error_code;
			}
			if (!ReadFile(pipe, reinterpret_cast<void*>(&buffer), sizeof(message), &bytes_read, NULL))
			{
				raise(GetLastError(), std::string("Failed to get a response to GET_SHARED message from server, bytes read: ") += std::to_string(bytes_read));
				return error_code;
			}

			if (buffer.type == NOT_FOUND)
			{
				std::cout << "Employee with specified ID does not exist\n";
				continue;
			}

			std::cout << "ID: " << buffer.data.id << '\n' << "Name: " << buffer.data.name << '\n' << "Hours: " << buffer.data.hours << '\n';

			buffer.type = message_types::UNLOCK_SHARED;
			temp_string = Query<std::string>("Press any key to stop access to the table entry:");

			if (!WriteFile(pipe, reinterpret_cast<void*>(&buffer), sizeof(message), &bytes_written, NULL))
			{
				raise(GetLastError(), std::string("Failed to write to named pipe when sending a UNLOCK_SHARED message, bytes written: ") += std::to_string(bytes_written));
				return error_code;
			}
			if (!ReadFile(pipe, reinterpret_cast<void*>(&buffer), sizeof(message), &bytes_read, NULL))
			{
				raise(GetLastError(), std::string("Failed to get a response to UNLOCK_SHARED message from server, bytes read: ") += std::to_string(bytes_read));
				return error_code;
			}

			continue;
		}

		if (temp_string[0] == 's')
		{
			buffer.id = Query<unsigned int>("Enter ID of the employee:");
			buffer.type = message_types::GET_EXCLUSIVE;

			if (!WriteFile(pipe, reinterpret_cast<void*>(&buffer), sizeof(message), &bytes_written, NULL))
			{
				raise(GetLastError(), std::string("Failed to write to named pipe when sending a GET_EXCLUSIVE message, bytes written: ") += std::to_string(bytes_written));
				return error_code;
			}
			if (!ReadFile(pipe, reinterpret_cast<void*>(&buffer), sizeof(message), &bytes_read, NULL))
			{
				raise(GetLastError(), std::string("Failed to get a response to GET_EXCLUSIVE message from server, bytes read: ") += std::to_string(bytes_read));
				return error_code;
			}

			if (buffer.type == NOT_FOUND)
			{
				std::cout << "Employee with specified ID does not exist\n";
				continue;
			}
			
			std::cout << "ID: " << buffer.data.id << '\n' << "Name: " << buffer.data.name << '\n' << "Hours: " << buffer.data.hours << '\n';

			temp_string = Query<std::string>("Enter new name:");
			strcpy_s(buffer.data.name, 20, temp_string.c_str());
			buffer.data.hours = Query<double>("Enter new hours:");
			buffer.type = message_types::SET;
			temp_string = Query<std::string>("Press any key to send new data to the server:");

			if (!WriteFile(pipe, reinterpret_cast<void*>(&buffer), sizeof(message), &bytes_written, NULL))
			{
				raise(GetLastError(), std::string("Failed to write to named pipe when sending a SET message, bytes written: ") += std::to_string(bytes_written));
				return error_code;
			}
			if (!ReadFile(pipe, reinterpret_cast<void*>(&buffer), sizeof(message), &bytes_read, NULL))
			{
				raise(GetLastError(), std::string("Failed to get a response to SET message from server, bytes read: ") += std::to_string(bytes_read));
				return error_code;
			}

			buffer.type = message_types::UNLOCK_EXCLUSIVE;
			temp_string = Query<std::string>("Press any key to stop access to the table entry:");

			if (!WriteFile(pipe, reinterpret_cast<void*>(&buffer), sizeof(message), &bytes_written, NULL))
			{
				raise(GetLastError(), std::string("Failed to write to named pipe when sending a UNLOCK_EXCLUSIVE message, bytes written: ") += std::to_string(bytes_written));
				return error_code;
			}
			if (!ReadFile(pipe, reinterpret_cast<void*>(&buffer), sizeof(message), &bytes_read, NULL))
			{
				raise(GetLastError(), std::string("Failed to get a response to UNLOCK_EXCLUSIVE message from server, bytes read: ") += std::to_string(bytes_read));
				return error_code;
			}
			continue;
		}

		buffer.type = message_types::SHUTDOWN;
		if (!WriteFile(pipe, reinterpret_cast<void*>(&buffer), sizeof(message), &bytes_written, NULL))
		{
			raise(GetLastError(), std::string("Failed to write to named pipe when sending a SHUTDOWN message, bytes written: ") += std::to_string(bytes_written));
			return error_code;
		}
		if (!ReadFile(pipe, reinterpret_cast<void*>(&buffer), sizeof(message), &bytes_read, NULL))
		{
			raise(GetLastError(), std::string("Failed to get a response to SHUTDOWN message from server, bytes read: ") += std::to_string(bytes_read));
			return error_code;
		}
		return 0ul;
	}
}
