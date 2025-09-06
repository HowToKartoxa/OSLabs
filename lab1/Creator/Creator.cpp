#include <iostream>
#include <fstream>

#include <Windows.h>

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
	while (employee_count > 0) 
	{
		std::cout << "Enter employee ID: ";
		std::cin >> temp_employee.num;
		std::cout << "Enter employee name: ";
		memset(temp_employee.name, 0, 10);
		fgets(temp_employee.name, 10, stdin);
		std::cout << "Enter employee hours: ";
		std::cin >> temp_employee.hours;
		out_stream.write((char*)(&temp_employee), sizeof(temp_employee));
		employee_count--;
	}

	out_stream.close();
}