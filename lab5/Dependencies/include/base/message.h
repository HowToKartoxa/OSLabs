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
	GET,
	GET_L,
	SET_L,
	RESP
};

#endif