#ifndef _EMPLOYEE_DATABASE
#define _EMPLOYEE_DATABASE

#include <utils/employee.h>

#include <string>
#include <vector>

#if defined(USE_WINAPI)
	#include <windows.h>
#elif defined(USE_BOOST)
	#include <boost/thread.hpp>
#endif

class EmployeeDB
{
	std::string name;

#if defined(USE_WINAPI)
	std::vector<SRWLOCK> locks;
#elif defined(USE_BOOST)
	std::vector<boost::shared_mutex*> mutexes;
#endif

	void acquire_shared(const size_t& index);
	void acquire_exclusive(const size_t& index);
	void release_shared(const size_t& index);
	void release_exclusive(const size_t& index);

public:

	EmployeeDB(std::string name, std::vector<Employee>& data);
	~EmployeeDB();

	bool WGetShared(size_t id, Employee& destination, size_t& locked_at);
	bool WGetExclusive(size_t id, Employee& destination, size_t& locked_at, size_t& found_at);
	bool Set(Employee& source, const size_t& found_at);
	bool UnlockShared(const size_t& index);
	bool UnlockExclusive(const size_t& index);
};

#endif
