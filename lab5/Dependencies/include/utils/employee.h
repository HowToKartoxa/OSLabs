#ifndef _EMPLOYEE_STRUCT
#define _EMPLOYEE_STRUCT

#include <string>
#include <string.h>

struct Employee
{
	const size_t id;
	char name[20];
	double hours;

	Employee() : id(0), hours(0), name("\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0") {}
	Employee(size_t _id, std::string _name, double _hours) : id(_id), hours(_hours), name("\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0")
	{
		for (size_t i = 0; i < _name.size() && i < 19; i++)
		{
			name[i] = _name[i];
		}
	}
	Employee(const Employee& src) : id(src.id), hours(src.hours)
	{
		for (size_t i = 0; i < 20; i++)
		{
			name[i] = src.name[i];
		}
	}
};

#endif