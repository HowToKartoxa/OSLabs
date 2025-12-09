#include <utils/employee_database.h>
#include <utils/employee_database_internal.h>

#include <fstream>
#include <vector>

EmployeeDB::EmployeeDB(std::string _name, std::vector<Employee>& data) : name(_name), locks()
{
	EmployeeDBInternal btree;
	for (size_t i = 0; i < data.size(); i++)
	{
		btree.Insert(data[i]);
	}

	size_t entry_count = btree.GetEntryCount();
	for (size_t i = 0; i < entry_count; i++)
	{
		locks.emplace_back();
		InitializeSRWLock(&locks[i]);
	}

	std::fstream file(name, std::ios::out | std::ios::in | std::ios::binary);
	btree.Serialize(file);
}

EmployeeDB::~EmployeeDB()
{
	std::remove(name.c_str());
}

DWORD EmployeeDB::WGetShared(size_t id, Employee& destination, size_t& index)
{
	std::fstream file(name, std::ios::in | std::ios::out | std::ios::binary);
	size_t found_at;
	if (EmployeeDBInternal::Find(id, file, index, found_at))
	{
		AcquireSRWLockShared(&locks[index]);
		EmployeeDBInternal::Get(file, found_at, destination);
		return 0ul;
	}
	else
	{
		return 1ul;
	}
}

DWORD EmployeeDB::WGetExclusive(size_t id, Employee& destination, size_t& index)
{
	std::fstream file(name, std::ios::in | std::ios::out | std::ios::binary);
	size_t found_at;
	if (EmployeeDBInternal::Find(id, file, index, found_at))
	{
		AcquireSRWLockExclusive(&locks[index]);
		EmployeeDBInternal::Get(file, found_at, destination);
		return 0ul;
	}
	else
	{
		return 1ul;
	}
}

DWORD EmployeeDB::Set(Employee& source, const size_t& index)
{
	std::fstream file(name, std::ios::out | std::ios::in | std::ios::binary);
	EmployeeDBInternal::Set(file, index, source);
}

DWORD EmployeeDB::UnlockShared(const size_t& index)
{
	ReleaseSRWLockShared(&locks[index]);
	return 0ul;
}

DWORD EmployeeDB::UnlockExclusive(const size_t& index)
{
	ReleaseSRWLockExclusive(&locks[index]);
	return 0ul;
}