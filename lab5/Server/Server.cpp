#include <base/server.h>
#include <utils/input_parsing.h>

#include <iostream>
#include <string>

#include <windows.h>

Server::Server() : active_connections(), database(nullptr) {}

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

	active_connections.resize(number_of_clients);
	for (unsigned short i = 0; i < number_of_clients; i++)
	{

	}
}