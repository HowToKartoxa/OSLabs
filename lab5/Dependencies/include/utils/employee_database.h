#ifndef _EMPLOYEE_DATABASE
#define _EMPLOYEE_DATABASE

#include <string>
#include <vector>

#include <windows.h>

class EmployeeDB
{
	std::string name;
	bool is_owner;
	std::vector<SRWLOCK>& locks;

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

	EmployeeDB(std::string name, std::vector<EmployeeDB::Employee>& data, std::vector<SRWLOCK>& locks);
	EmployeeDB(std::string name, std::vector<SRWLOCK>& locks);
	~EmployeeDB();
	DWORD WGet(unsigned int id, Employee& destination);
	DWORD WSet(unsigned int id, Employee& source);
	DWORD WGetAndLock(unsigned int id, Employee& destination, size_t& index);
	DWORD WSetAndUnlock(unsigned int id, Employee& source, const size_t& index);
	DWORD WUnlock(const size_t& index);
};

#endif
