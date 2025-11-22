#include <utils/employee_database.h>

#include <fstream>

EmployeeDB::EmployeeDB(std::string _name, Employee* data, int _count, bool own = true) : name(_name), count(_count), is_owner(own), write_mutexes(nullptr)
{
	if (is_owner)
	{
		write_mutexes = new HANDLE[count];
		for (unsigned int i = 0; i < count; i++)
		{
			write_mutexes[i] = CreateMutexA(NULL, FALSE, (name + "_TABLE_INDEX_" + std::to_string(i) + "_WRITE_MUTEX").c_str());
			if (write_mutexes[i] == INVALID_HANDLE_VALUE)
			{

			}
		}
		Employee placeholder;
		std::fstream file(name, std::ios::in | std::ios::out);
		if (!file.is_open())
		{

		}
		
	}
	else
	{
		
	}
}

EmployeeDB::~EmployeeDB()
{
	if (is_owner)
	{

	}
}

DWORD EmployeeDB::Get(int id, Employee& destination)
{

}

DWORD EmployeeDB::Set(int id, const Employee& source)
{

}