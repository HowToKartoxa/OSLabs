#define BOOST_TEST_MODULE main_tests

#include <utils/employee_database.h>
#include <utils/employee_database_internal.h>

#include <vector>
#include <string>
#include <fstream>

#include <boost/test/included/unit_test.hpp>
#include <windows.h>

DWORD WINAPI test_set(LPVOID params)
{
	EmployeeDB* temp = reinterpret_cast<EmployeeDB*>(params);
	Employee temp_employee;
	size_t locked_at, found_at;
	temp->WGetExclusive(1, temp_employee, locked_at, found_at);
	return 0ul;
}

DWORD WINAPI test_get(LPVOID params)
{
	EmployeeDB* temp = reinterpret_cast<EmployeeDB*>(params);
	Employee temp_employee;
	size_t locked_at;
	temp->WGetShared(1, temp_employee, locked_at);
	return 0ul;
}

BOOST_AUTO_TEST_SUITE(employee_db_test)

BOOST_AUTO_TEST_CASE(employee_db_small_init)
{
	std::vector<Employee> test_data;
	for (size_t i = 0; i < 5; i++)
	{
		test_data.emplace_back(i, std::to_string(i), static_cast<double>(i));
	}
	EmployeeDB database("test_db", test_data);
	std::fstream file("test_db", std::ios::in);
	BOOST_TEST(file.is_open() == true);
}

BOOST_AUTO_TEST_CASE(employee_db_large_init)
{
	std::vector<Employee> test_data;
	for (size_t i = 0; i < 2000; i++)
	{
		test_data.emplace_back(i, std::to_string(i), static_cast<double>(i));
	}
	EmployeeDB database("test_db", test_data);
	std::fstream file("test_db", std::ios::in);
	BOOST_TEST(file.is_open() == true);
}

BOOST_AUTO_TEST_CASE(employee_db_no_duplicates)
{
	std::vector<Employee> test_data;
	test_data.emplace_back(1ull, std::string("test_employee"), 10.0);
	test_data.emplace_back(1ull, std::string("duplicate_test_employee"), 8.0);
	EmployeeDB database("test_db", test_data);
	std::fstream file("test_db", std::ios::in | std::ios::ate | std::ios::binary);
	size_t size = EmployeeDBInternal::GetPageSize() + sizeof(Employee) + sizeof(size_t);
	size_t act_size = file.tellg();
	BOOST_TEST(size == act_size);
}

BOOST_AUTO_TEST_CASE(employee_db_small_get)
{
	std::vector<Employee> test_data;
	for (size_t i = 0; i < 5; i++)
	{
		test_data.emplace_back(i, std::to_string(i), static_cast<double>(i));
	}
	EmployeeDB database("test_db", test_data);
	Employee temp_employee;
	size_t locked_at;
	
	BOOST_TEST(database.WGetShared(3ull, temp_employee, locked_at) == false);
	BOOST_TEST(temp_employee.id == 3ull);
	int res = strcmp(temp_employee.name, "3");
	BOOST_TEST(res == 0);
	BOOST_TEST(temp_employee.hours == 3.0);
	BOOST_TEST(database.WGetShared(10ull, temp_employee, locked_at) == true);
}

BOOST_AUTO_TEST_CASE(employee_db_large_get)
{
	std::vector<Employee> test_data;
	for (size_t i = 0; i < 2000; i++)
	{
		test_data.emplace_back(i, std::to_string(i), static_cast<double>(i));
	}
	EmployeeDB database("test_db", test_data);
	Employee temp_employee;
	size_t locked_at;
	BOOST_TEST(database.WGetShared(1900ull, temp_employee, locked_at) == false);
	BOOST_TEST(temp_employee.id == 1900ull);
	int res = strcmp(temp_employee.name, "1900");
	BOOST_TEST(res == 0);
	BOOST_TEST(temp_employee.hours == 1900);
	BOOST_TEST(database.WGetShared(10000ull, temp_employee, locked_at) == true);
}

BOOST_AUTO_TEST_CASE(employee_db_small_set)
{
	std::vector<Employee> test_data;
	for (size_t i = 0; i < 5; i++)
	{
		test_data.emplace_back(i, std::to_string(i), static_cast<double>(i));
	}
	EmployeeDB database("test_db", test_data);
	Employee temp_employee;
	size_t locked_at, found_at;
	database.WGetExclusive(1ull, temp_employee, locked_at, found_at);
	strcpy_s(temp_employee.name, 8, "new_val\0");
	temp_employee.hours = 10.0;
	database.Set(temp_employee, found_at);
	database.UnlockExclusive(locked_at);
	database.WGetShared(temp_employee.id, temp_employee, locked_at);
	BOOST_TEST(temp_employee.id == 1ull);
	int res = strcmp(temp_employee.name, "new_val");
	BOOST_TEST(res == 0);
	BOOST_TEST(temp_employee.hours == 10.0);
}

BOOST_AUTO_TEST_CASE(employee_db_large_set)
{
	std::vector<Employee> test_data;
	for (size_t i = 0; i < 2000; i++)
	{
		test_data.emplace_back(i, std::to_string(i), static_cast<double>(i));
	}
	EmployeeDB database("test_db", test_data);
	Employee temp_employee;
	size_t locked_at, found_at;
	database.WGetExclusive(1900ull, temp_employee, locked_at, found_at);
	strcpy_s(temp_employee.name, 8, "new_val\0");
	temp_employee.hours = 10.0;
	database.Set(temp_employee, found_at);
	database.UnlockExclusive(locked_at);
	database.WGetShared(temp_employee.id, temp_employee, locked_at);
	BOOST_TEST(temp_employee.id == 1900ull);
	int res = strcmp(temp_employee.name, "new_val");
	BOOST_TEST(res == 0);
	BOOST_TEST(temp_employee.hours == 10.0);
}

BOOST_AUTO_TEST_CASE(employee_db_set_lock)
{
	std::vector<Employee> test_data;
	for (size_t i = 0; i < 5; i++)
	{
		test_data.emplace_back(i, std::to_string(i), static_cast<double>(i));
	}
	EmployeeDB database("test_db", test_data);
	Employee temp_employee;
	size_t locked_at;
	DWORD id;
	database.WGetShared(1, temp_employee, locked_at);
	HANDLE thread = CreateThread(NULL, NULL, test_set, &database, NULL, &id);
	BOOST_TEST(thread != nullptr);
	if (thread != NULL)
	{
		DWORD res = WaitForSingleObject(thread, 2000);
		BOOST_TEST(res == WAIT_TIMEOUT);
	}
}

BOOST_AUTO_TEST_CASE(employee_db_get_lock)
{
	std::vector<Employee> test_data;
	for (size_t i = 0; i < 5; i++)
	{
		test_data.emplace_back(i, std::to_string(i), static_cast<double>(i));
	}
	EmployeeDB database("test_db", test_data);
	Employee temp_employee;
	size_t locked_at, found_at;
	DWORD id;
	database.WGetExclusive(1, temp_employee, locked_at, found_at);
	HANDLE thread = CreateThread(NULL, NULL, test_get, &database, NULL, &id);
	BOOST_TEST(thread != nullptr);
	if (thread != NULL)
	{
		DWORD res = WaitForSingleObject(thread, 2000);
		BOOST_TEST(res == WAIT_TIMEOUT);
	}
}
BOOST_AUTO_TEST_SUITE_END()
