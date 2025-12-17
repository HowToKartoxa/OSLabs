#if defined(USE_WINAPI)

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
		std::cout << client.GetErrorMessage() << '\n';
		std::system("pause");
		return client.GetErrorCode();
	}
}

#elif defined(USE_BOOST)

#include <base/client_boost.h>

#include <iostream>
#include <cstdlib>

int main(int argc, char** argv)
{
	if (argc < 3)
	{
		return 1ul;
	}

	Client client(argv[2], atoi(argv[1]));
	try
	{
		client.Operate();
	}
	catch (boost::interprocess::interprocess_exception e)
	{
		std::cout << "Error: " << e.what() << '\n';
		std::system("pause");
		return 1;
	}

	std::system("pause");
	return 0;
}

#endif

