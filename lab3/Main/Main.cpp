#if defined(USE_WINAPI)

#include <utils/thread_manager.h>

#elif defined(USE_BOOST)

#include <utils/thread_manager_boost.h>

#endif

#include <utils/input_parsing.h>

#include <iostream>
#include <string>

int main(int arc, char** argv) 
{
	std::string temp_string;

	std::cout << "Enter array size:\n";
	std::getline(std::cin, temp_string);
	while (!CheckIfSizeT(temp_string))
	{
		std::cout << "Enter array size:\n";
		std::getline(std::cin, temp_string);
	}

	size_t size = StringToSizeT(temp_string);
	int* array = new int[size];
	for (size_t i = 0; i < size; i++) 
	{
		array[i] = 0;
	}

	std::cout << "Enter number of marker threads:\n";
	std::getline(std::cin, temp_string);
	while (!CheckIfUnsignedShort(temp_string)) 
	{
		std::cout << "Enter number of marker threads:\n";
		std::getline(std::cin, temp_string);
	}
	
	ThreadManager* manager;

	try 
	{
		manager = new ThreadManager(array, size, std::stoi(temp_string));
	}
	catch (DWORD exception) 
	{
		return exception;
	}

	return manager->Operate();
}