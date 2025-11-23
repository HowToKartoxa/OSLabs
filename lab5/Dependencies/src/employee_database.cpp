#include <utils/employee_database.h>

#include <fstream>
#include <algorithm>
#include <vector>

EmployeeDB::EmployeeDB(std::string _name, std::vector<EmployeeDB::Employee>& data, std::vector<SRWLOCK>& _locks) : locks(_locks), name(_name), is_owner(true)
{
	unsigned int size = data.size();

	std::fstream file(name, std::ios::out);
	if (!file.is_open())
	{

	}
	file.write(reinterpret_cast<char*>(size), sizeof(unsigned int));
	for (unsigned int i = 0u; i < size; i++)
	{
		file.write(reinterpret_cast<char*>(&data[i]), sizeof(Employee));
	}
	file.close();
}

EmployeeDB::EmployeeDB(std::string _name, std::vector<SRWLOCK>& _locks) : locks(_locks), name(_name), is_owner(false){}

EmployeeDB::~EmployeeDB()
{
	if (is_owner)
	{
		std::remove(name.c_str());
	}
}

DWORD EmployeeDB::WGet(unsigned int id, Employee& destination)
{
	std::fstream file(name, std::ios::in);
	unsigned int size;
	file.read(reinterpret_cast<char*>(&size), sizeof(unsigned int));
	for (unsigned int i = 0u; i < size; i++)
	{
		AcquireSRWLockShared(&locks[i]);
		file.read(reinterpret_cast<char*>(&destination), sizeof(Employee));
		ReleaseSRWLockShared(&locks[i]);
		if (destination.id == id)
		{
			file.close();
			return 0ul;
		}
	}
	file.close();
	return 1ul;
}

DWORD EmployeeDB::WSet(unsigned int id, Employee& source)
{
	std::fstream file(name, std::ios::in | std::ios::out);
	unsigned int size;
	Employee temp;
	file.read(reinterpret_cast<char*>(&size), sizeof(unsigned int));
	for (size_t i = 0ull; i < size; i++)
	{
		AcquireSRWLockShared(&locks[i]);
		file.read(reinterpret_cast<char*>(&temp), sizeof(Employee));
		ReleaseSRWLockShared(&locks[i]);
		if (temp.id == id)
		{
			file.seekp(sizeof(unsigned int) + i * sizeof(Employee));
			AcquireSRWLockExclusive(&locks[i]);
			file.write(reinterpret_cast<char*>(&source), sizeof(Employee));
			ReleaseSRWLockExclusive(&locks[i]);
			file.close();
			return 0ul;
		}
	}
	file.close();
	return 1ul;
}

DWORD EmployeeDB::WGetAndLock(unsigned int id, Employee& destination, size_t& index)
{
	std::fstream file(name, std::ios::in);
	unsigned int size;
	file.read(reinterpret_cast<char*>(&size), sizeof(unsigned int));
	for (unsigned int i = 0u; i < size; i++)
	{
		AcquireSRWLockShared(&locks[i]);
		file.read(reinterpret_cast<char*>(&destination), sizeof(Employee));
		ReleaseSRWLockShared(&locks[i]);
		if (destination.id == id)
		{
			AcquireSRWLockExclusive(&locks[i]);
			index = i;
			file.close();
			return 0ul;
		}
	}
	file.close();
	return 1ul;
}

DWORD EmployeeDB::WSetAndUnlock(unsigned int id, Employee& source, size_t& index)
{
	std::fstream file(name, std::ios::in);
	file.seekp(sizeof(unsigned int) + index * sizeof(Employee));
	file.write(reinterpret_cast<char*>(&source), sizeof(Employee));
	ReleaseSRWLockExclusive(&locks[index]);
	return 0ul;
}