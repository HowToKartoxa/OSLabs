#include <base/client.h>

#include <iostream>
#include <cstdlib>

#include <windows.h>

DWORD main(int argc, char** argv)
{
	if (argc < 3)
	{
		return 1ul;
	}
	
	Client client(argv[2], atoi(argv[1]));
	client.Operate();

	if (client.Ok())
	{
		return 0ul;
	}
	else
	{
		std::cout << client.GetErrorMessage();
		std::system("pause");
		return client.GetErrorCode();
	}
}