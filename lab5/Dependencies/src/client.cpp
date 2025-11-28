#include <base/client.h>
#include <base/message.h>

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
	bool go_on;

	std::cout << "Enter 'g' to get data, 's' to get and change data, anything else to disconnect from the server:\n";

	std::getline(std::cin, temp_string);
	if (temp_string.empty())
	{
		break;
	}
	else
	{
		switch (temp_string[0])
		{
			case 'g': 
			{

				break;
			}
			case 's':
			{

				break;
			}
			default:
			{
				
			}
		}
	}
}
