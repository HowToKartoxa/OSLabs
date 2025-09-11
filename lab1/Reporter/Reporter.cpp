#include "utils/employee.h"

#include <iostream>
#include <fstream>
#include <algorithm>
#include <string>
#include <cstring>

int main(int argc, char** argv)
{
	std::cout << "Reporter utility\nUsage: Reporter [source_binary_file_name] [result_txt_file_name] [hourly_payment_ammount]\n";
	if (argc < 4) 
	{
		std::cout << "Too few arguments passed!";
		system("pause");
		return 1;
	}

	std::ifstream in_stream(argv[1], std::ios::binary | std::ios::in);

	if (!in_stream.good()) 
	{
		std::cout
			<< "Failed to open binary file "
			<< argv[1]
			<< "! (Check if this file exists.)\n";
		system("pause");
		return 1;
	}

	std::ofstream out_stream(argv[2], std::ios::out);

	if (!out_stream.good()) 
	{
		std::cout
			<< "Failed to create text file "
			<< argv[2]
			<< "! (Try passing a different file name next time.)\n";
		system("pause");
		return 1;
	}

	double hourly_payment = std::stod(argv[3]);

	size_t employee_count;
	in_stream.read((char*)(&employee_count), sizeof(employee_count));

	employee* employees = new employee[employee_count];

	for (size_t i = 0; i < employee_count; i++) 
	{
		in_stream.read((char*)(&employees[i]), sizeof(employees[i]));
	}
	in_stream.close();

	std::sort(employees, employees + employee_count, [](employee a, employee b) {return strcmp(a.name, b.name) < 0; });

	out_stream
		<< "Report for " << argv[1] << ":\n"
		<< "ID\tName\tHours\tSalary\n";
	for (size_t i = 0; i < employee_count; i++) 
	{
		out_stream
			<< employees[i].num << '\t'
			<< employees[i].name << '\t'
			<< employees[i].hours << '\t'
			<< employees[i].hours * hourly_payment << '\n';
	}

	delete[] employees;

	out_stream.close();

	return 0;
}