#include <iostream>
#include <fstream>
#include <string>

struct employee 
{
	int num;
	char name[10];
	double hours;
};

int main(int argc, char** argv) 
{
	std::ofstream out_stream(argv[1], std::ios::binary | std::ios::out);
	size_t employee_count = std::atoi(argv[2]);

	out_stream.write((char*)(&employee_count), sizeof(employee_count));

	employee temp_employee;
	std::string temp_string;
	while (employee_count > 0) 
	{
		std::cout << "Enter employee ID: ";
		std::cin >> temp_employee.num;
		std::cout << "Enter employee name: ";
		std::cin.ignore();
		std::getline(std::cin, temp_string);
		strcpy_s(temp_employee.name, temp_string.c_str());
		std::cout << "Enter employee hours: ";
		std::cin >> temp_employee.hours;
		out_stream.write((char*)(&temp_employee), sizeof(temp_employee));
		employee_count--;
	}

	out_stream.close();
}