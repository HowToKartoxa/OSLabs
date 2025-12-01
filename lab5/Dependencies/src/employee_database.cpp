#include <utils/employee_database.h>

#include <fstream>
#include <vector>

EmployeeDB::EmployeeDB(std::string _name, std::vector<EmployeeDB::Employee>& data, std::vector<SRWLOCK>& _locks) : locks(_locks), name(_name)
{
	unsigned int size = data.size();

	std::fstream file(name, std::ios::out | std::ios::binary);
	if (!file.is_open())
	{

	}
	file.write(reinterpret_cast<char*>(&size), sizeof(unsigned int));
	for (unsigned int i = 0u; i < size; i++)
	{
		file.write(reinterpret_cast<char*>(&data[i]), sizeof(Employee));
	}
	file.close();
}

EmployeeDB::~EmployeeDB()
{
	std::remove(name.c_str());
}

DWORD EmployeeDB::WGetShared(unsigned int id, Employee& destination, size_t& index)
{
	std::fstream file(name, std::ios::in | std::ios::binary);
	unsigned int size;
	file.read(reinterpret_cast<char*>(&size), sizeof(unsigned int));
	for (unsigned int i = 0u; i < size; i++)
	{
		AcquireSRWLockShared(&locks[i]);
		file.read(reinterpret_cast<char*>(&destination), sizeof(Employee));
		ReleaseSRWLockShared(&locks[i]);
		if (destination.id == id)
		{
			AcquireSRWLockShared(&locks[i]);
			index = i;
			file.close();
			return 0ul;
		}
	}
	file.close();
	return 1ul;
}

DWORD EmployeeDB::WGetExclusive(unsigned int id, Employee& destination, size_t& index)
{
	std::fstream file(name, std::ios::in | std::ios::binary);
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

DWORD EmployeeDB::Set(Employee& source, const size_t& index)
{
	std::fstream file(name, std::ios::out | std::ios::in | std::ios::binary);
	file.seekp(sizeof(unsigned int) + index * sizeof(Employee));
	file.write(reinterpret_cast<char*>(&source), sizeof(Employee));
	return 0ul;
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