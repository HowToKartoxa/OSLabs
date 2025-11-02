#include <utils/input_parsing.h>
#include <utils/message_queue.h>

#include <iostream>
#include <string>
#include <fstream>

int main(int argc, char** argv) 
{
	std::string temp_string;

	std::cout << "Enter the binary file name:\n";
	std::getline(std::cin, temp_string);
	while (!CheckFileName(temp_string, ""))
	{
		std::cout << "Enter the binary file name:\n";
		std::getline(std::cin, temp_string);
	}

	std::fstream binary_file(temp_string, std::ios::binary);

	std::cout << "Enter the number of message entries in the binary file:\n";
	std::getline(std::cin, temp_string);
	while (!CheckIfSizeT(temp_string))
	{
		std::cout << "Enter the number of message entries in the binary file:\n";
		std::getline(std::cin, temp_string);
	}

	size_t number_of_file_entries = StringToSizeT(temp_string);

}
