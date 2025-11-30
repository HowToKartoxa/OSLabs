#include <base/client.h>

int main(int argc, char** argv)
{
	if (argc < 3)
	{
		return 1;
	}
	
	Client client(argv[2], atoi(argv[1]));
	return client.Operate();
}