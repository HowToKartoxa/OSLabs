#include <base/server_boost.h>

void Server::Operate()
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
}