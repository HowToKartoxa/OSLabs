#include <base/server.h>

#include <iostream>
#include <cstdlib>

#include <windows.h>

DWORD main(int argc, char** argv)
{
	Server server;
	server.Operate();

	if (server.Ok())
	{
		std::cout << "hui";
		std::system("pause");
		return 0;
	}
	else
	{
		std::cout << server.GetErrorMessage();
		std::system("pause");
		return server.GetErrorCode();
	}
}