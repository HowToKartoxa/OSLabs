#include <base/client_boost.h>
#include <base/message.h>
#include <utils/input_parsing.h>
#include <utils/employee.h>

#include <iostream>
#include <string>

Client::Client(char* mq_name, unsigned short _client_number) : 
	in(boost::interprocess::open_only, ((std::string(mq_name) + "out_") + std::to_string(_client_number)).c_str()), 
	out(boost::interprocess::open_only, ((std::string(mq_name) + "in_") + std::to_string(_client_number)).c_str()),
	client_number(_client_number) {}

Client::~Client() {}

void Client::Operate()
{
	std::cout << "Client [" << client_number << "]\n";
	std::string temp_string = "";
	message buffer(message_types::FOUND, 0, Employee(0, "", 0));
	size_t bytes_recieved;
	unsigned int priority = 0;

	while (true)
	{
		temp_string = Query<std::string>("Enter 'g' to get table entry by id, 's' to get and change table entry by id, enter anything else to disconnect from the server:");

		if (temp_string.empty())
		{
			buffer.type = message_types::SHUTDOWN;
			try
			{
				out.send(&buffer, sizeof(message), priority);
			}
			catch (boost::interprocess::interprocess_exception e)
			{
				throw e;
			}
			try
			{
				in.receive(&buffer, sizeof(message), bytes_recieved, priority);
			}
			catch (boost::interprocess::interprocess_exception e)
			{
				throw e;
			}
			return;
		}

		if (temp_string[0] == 'g')
		{
			buffer.id = Query<size_t>("Enter id of the employee:");
			buffer.type = message_types::GET_SHARED;

			try
			{
				out.send(&buffer, sizeof(message), priority);
			}
			catch (boost::interprocess::interprocess_exception e)
			{
				throw e;
			}
			try
			{
				in.receive(&buffer, sizeof(message), bytes_recieved, priority);
			}
			catch (boost::interprocess::interprocess_exception e)
			{
				throw e;
			}

			if (buffer.type == NOT_FOUND)
			{
				std::cout << "Employee with specified ID does not exist\n";
				continue;
			}

			std::cout << "ID: " << buffer.data.id << '\n' << "Name: " << buffer.data.name << '\n' << "Hours: " << buffer.data.hours << '\n';

			buffer.type = message_types::UNLOCK_SHARED;
			temp_string = Query<std::string>("Press any key to stop access to the table entry:");

			try
			{
				out.send(&buffer, sizeof(message), priority);
			}
			catch (boost::interprocess::interprocess_exception e)
			{
				throw e;
			}
			try
			{
				in.receive(&buffer, sizeof(message), bytes_recieved, priority);
			}
			catch (boost::interprocess::interprocess_exception e)
			{
				throw e;
			}

			continue;
		}

		if (temp_string[0] == 's')
		{
			buffer.id = Query<unsigned int>("Enter ID of the employee:");
			buffer.type = message_types::GET_EXCLUSIVE;

			try
			{
				out.send(&buffer, sizeof(message), priority);
			}
			catch (boost::interprocess::interprocess_exception e)
			{
				throw e;
			}
			try
			{
				in.receive(&buffer, sizeof(message), bytes_recieved, priority);
			}
			catch (boost::interprocess::interprocess_exception e)
			{
				throw e;
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

			try
			{
				out.send(&buffer, sizeof(message), priority);
			}
			catch (boost::interprocess::interprocess_exception e)
			{
				throw e;
			}
			try
			{
				in.receive(&buffer, sizeof(message), bytes_recieved, priority);
			}
			catch (boost::interprocess::interprocess_exception e)
			{
				throw e;
			}

			buffer.type = message_types::UNLOCK_EXCLUSIVE;
			temp_string = Query<std::string>("Press any key to stop access to the table entry:");

			try
			{
				out.send(&buffer, sizeof(message), priority);
			}
			catch (boost::interprocess::interprocess_exception e)
			{
				throw e;
			}
			try
			{
				in.receive(&buffer, sizeof(message), bytes_recieved, priority);
			}
			catch (boost::interprocess::interprocess_exception e)
			{
				throw e;
			}
			continue;
		}

		buffer.type = message_types::SHUTDOWN;
		try
		{
			out.send(&buffer, sizeof(message), priority);
		}
		catch (boost::interprocess::interprocess_exception e)
		{
			throw e;
		}
		try
		{
			in.receive(&buffer, sizeof(message), bytes_recieved, priority);
		}
		catch (boost::interprocess::interprocess_exception e)
		{
			throw e;
		}
		return;
	}
}