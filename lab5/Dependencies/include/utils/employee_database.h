#ifndef _EMPLOYEE_DATABASE
#define _EMPLOYEE_DATABASE

#include <string>

#include <windows.h>

class EmployeeDB
{
	std::string name;
	unsigned int count;
	bool is_owner;
	HANDLE* write_mutexes;

public:
	struct Employee
	{
		unsigned int id;
		char name[20];
		double hours;

		Employee() : id(0), hours(0), name("\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0") {}
		Employee(int _id, std::string _name, double _hours) : id(_id), hours(_hours), name("\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0")
		{
			for (size_t i = 0; i < _name.length() && i < 19; i++)
			{
				name[i] = _name[i];
			}
		}
	};

	EmployeeDB(std::string _name, Employee* data, int _count, bool own = true);
	~EmployeeDB();
	DWORD Get(int id, Employee& destination);
	DWORD Set(int id, const Employee& source);
};

#endif
