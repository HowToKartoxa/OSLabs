#ifndef _FILE_ACCESS_MESSAGE
#define _FILE_ACCESS_MESSAGE

#include <utils/employee_database.h>
#include <utils/employee.h>

struct message
{
	char type;
	size_t id;
	Employee data;

	message(char _type, size_t _id, Employee _data) : type(_type), id(_id), data(_data) {}
};

enum message_types
{
	GET_SHARED,
	GET_EXCLUSIVE,
	SET,
	UNLOCK_SHARED,
	UNLOCK_EXCLUSIVE,
	SHUTDOWN,
	NOT_FOUND,
	FOUND
};

#endif