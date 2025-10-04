#include <utils/input_parsing.h>
#include <utils/marker_procedures.h>
#include <utils/marker_parameter_data.h>

#include <iostream>
#include <string>

#include <windows.h>

int main(int arc, char** argv) 
{
	size_t size;
	std::string temp_string;

	std::cout << "Enter array size:\n";
	std::getline(std::cin, temp_string);
	while (!CheckIfSizeT(temp_string))
	{
		std::cout << "Enter array size:\n";
		std::getline(std::cin, temp_string);
	}

	size = StringToSizeT(temp_string);

	for (size_t i = 0; i < size; i++) 
	{

	}
}