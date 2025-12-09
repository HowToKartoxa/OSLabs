#ifndef _EMPLOYEE_DATABASE
#define _EMPLOYEE_DATABASE

#include <utils/employee.h>

#include <string>
#include <vector>

#include <windows.h>

class EmployeeDB
{
	std::string name;
	std::vector<SRWLOCK> locks;

public:

	EmployeeDB(std::string name, std::vector<Employee>& data);
	~EmployeeDB();

	DWORD WGetShared(size_t id, Employee& destination, size_t& index);
	DWORD WGetExclusive(size_t id, Employee& destination, size_t& index);
	DWORD Set(Employee& source, const size_t& index);
	DWORD UnlockShared(const size_t& index);
	DWORD UnlockExclusive(const size_t& index);
};

#endif
