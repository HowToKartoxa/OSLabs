#include <utils/employee_database.h>
#include <utils/employee_database_internal.h>

#include <fstream>
#include <vector>

EmployeeDB::EmployeeDB(std::string _name, std::vector<Employee>& data) : 
	name(_name),
#if defined(USE_WINAPI)
	locks()
#elif defined(USE_BOOST)
	mutexes()
#endif
{

	EmployeeDBInternal btree;
	for (size_t i = 0; i < data.size(); i++)
	{
		btree.Insert(data[i]);
	}

	size_t entry_count = btree.GetEntryCount();

#if defined (USE_WINAPI)
	for (size_t i = 0; i < entry_count; i++)
	{
		locks.emplace_back();
		InitializeSRWLock(&locks[i]);
	}
#elif defined(USE_BOOST)
	for (size_t i = 0; i < entry_count; i++)
	{
		mutexes.push_back(new boost::shared_mutex());
	}
#endif

	std::fstream file(name, std::ios::out | std::ios::binary);
	btree.Serialize(file);
}

EmployeeDB::~EmployeeDB()
{
#if defined(USE_BOOST)
	for (size_t i = 0; i < mutexes.size(); i++)
	{
		delete mutexes[i];
	}
#endif
	std::remove(name.c_str());
}

void EmployeeDB::acquire_shared(const size_t& index)
{

#if defined (USE_WINAPI)
	AcquireSRWLockShared(&locks[index]);
#elif defined(USE_BOOST)
	mutexes[index]->lock_shared();
#endif

}

void EmployeeDB::acquire_exclusive(const size_t& index)
{

#if defined (USE_WINAPI)
	AcquireSRWLockExclusive(&locks[index]);
#elif defined(USE_BOOST)
	mutexes[index]->lock();
#endif

}

void EmployeeDB::release_shared(const size_t& index)
{

#if defined (USE_WINAPI)
	AcquireSRWLockShared(&locks[index]);
#elif defined(USE_BOOST)
	mutexes[index]->unlock_shared();
#endif

}

void EmployeeDB::release_exclusive(const size_t& index)
{

#if defined (USE_WINAPI)
	AcquireSRWLockShared(&locks[index]);
#elif defined(USE_BOOST)
	mutexes[index]->unlock();
#endif

}

bool EmployeeDB::WGetShared(size_t id, Employee& destination, size_t& locked_at)
{
	std::fstream file(name, std::ios::in | std::ios::binary);
	size_t found_at;
	if (EmployeeDBInternal::Find(id, file, locked_at, found_at))
	{
		acquire_shared(locked_at);
		EmployeeDBInternal::Get(file, found_at, destination);
		return false;
	}
	else
	{
		return true;
	}
}

bool EmployeeDB::WGetExclusive(size_t id, Employee& destination, size_t& locked_at, size_t& found_at)
{
	std::fstream file(name, std::ios::in | std::ios::binary);
	if (EmployeeDBInternal::Find(id, file, locked_at, found_at))
	{
		acquire_exclusive(locked_at);
		EmployeeDBInternal::Get(file, found_at, destination);
		return false;
	}
	else
	{
		return true;
	}
}

bool EmployeeDB::Set(Employee& source, const size_t& found_at)
{
	std::fstream file(name, std::ios::out | std::ios::in | std::ios::binary);
	EmployeeDBInternal::Set(file, found_at, source);
	return false;
}

bool EmployeeDB::UnlockShared(const size_t& index)
{
	release_shared(index);
	return false;
}

bool EmployeeDB::UnlockExclusive(const size_t& index)
{
	release_shared(index);
	return false;
}