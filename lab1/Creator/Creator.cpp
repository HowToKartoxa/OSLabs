#include "utils/input_parsing.h"
#include "utils/employee.h"

#include <iostream>
#include <fstream>
#include <string>

#include <windows.h>

int main(int argc, char** argv) 
{
	std::cout << "Creator utility\nUsage: Creator [result_binary_file_name] [binary_file_entry_count]\n";
	if (argc < 2) 
	{
		std::cout << "Too few arguments passed!\n";
		system("pause");
		return 1;
	}

	std::ofstream out_stream(argv[1], std::ios::binary | std::ios::out);

	if (!out_stream.good())
	{
		std::cout
			<< "Failed to create binary file "
			<< argv[1]
			<< "! (Try passing a different file name next time.)\n";
		system("pause");
		return 1;
	}

	size_t employee_count = StringToSizeT(std::string(argv[2]));

	out_stream.write((char*)(&employee_count), sizeof(employee_count));

	employee temp_employee;
	std::string temp_string;

	while (employee_count > 0) 
	{
		std::cout << "Enter employee ID:\n";
		std::getline(std::cin, temp_string);
		while (!CheckIfInt(temp_string)) 
		{
			std::cout << "Enter employee ID:\n";
			std::getline(std::cin, temp_string);
		}
		temp_employee.num = std::stoi(temp_string.c_str());

		std::cout << "Enter employee name:\n";
		std::getline(std::cin, temp_string);
		strcpy_s(temp_employee.name, temp_string.c_str());

		std::cout << "Enter employee hours:\n";
		std::getline(std::cin, temp_string);
		while (!CheckIfDouble(temp_string)) 
		{
			std::cout << "Enter employee hours:\n";
			std::getline(std::cin, temp_string);
		}
		temp_employee.hours = std::stod(temp_string);

		out_stream.write((char*)(&temp_employee), sizeof(temp_employee));
		employee_count--;
	}

	out_stream.close();
	return 0;
}