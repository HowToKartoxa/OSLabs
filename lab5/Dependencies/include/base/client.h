#ifndef _FILE_ACCESS_CLIENT
#define _FILE_ACCESS_CLIENT

#include <string>

#include <windows.h>

class Client
{
	HANDLE pipe;
	unsigned short client_number;

	DWORD error_code;
	std::string error_message;

public:
	Client(char* pipe_name, unsigned short _client_number);
	~Client();
	DWORD Operate();

	bool Ok();
	DWORD GetErrorCode();
	std::string GetErrorMessage();

private:
	void raise(DWORD code, std::string message);
};

#endif
