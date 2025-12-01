#ifndef _FILE_ACCESS_MESSAGE
#define _FILE_ACCESS_MESSAGE

#include <utils/employee_database.h>

struct message
{
	char type;
	EmployeeDB::Employee data;

	message(char _type, EmployeeDB::Employee _data) : type(_type), data(_data) {}
};

enum message_types
{
	GET_SHARED,
	GET_EXCLUSIVE,
	SET,
	UNLOCK_SHARED,
	UNLOCK_EXCLUSIVE,
	NOT_FOUND,
	FOUND
};

#endif