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
		std::system("pause");
		return 0ul;
	}
	else
	{
		std::cout << server.GetErrorMessage() << '\n';
		std::system("pause");
		return server.GetErrorCode();
	}
}