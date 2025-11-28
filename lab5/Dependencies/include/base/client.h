#ifndef _FILE_ACCESS_CLIENT
#define _FILE_ACCESS_CLIENT

#include <windows.h>

class Client
{
	HANDLE pipe;
	unsigned short client_number;

public:
	Client(char* pipe_name, unsigned short _client_number);
	~Client();
	DWORD Operate();
};

#endif
