#ifndef _EMPLOYEE_DATABASE
#define _EMPLOYEE_DATABASE

#include <utils/employee.h>

#include <string>
#include <vector>

#include <windows.h>

class EmployeeDB
{
	std::string name;
	std::vector<SRWLOCK>& locks;

public:

	EmployeeDB(std::string name, std::vector<Employee>& data, std::vector<SRWLOCK>& locks);
	~EmployeeDB();

	DWORD WGetShared(unsigned int id, Employee& destination, size_t& index);
	DWORD WGetExclusive(unsigned int id, Employee& destination, size_t& index);
	DWORD Set(Employee& source, const size_t& index);
	DWORD UnlockShared(const size_t& index);
	DWORD UnlockExclusive(const size_t& index);
};

#endif
