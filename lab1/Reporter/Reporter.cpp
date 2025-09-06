#include <iostream>
#include <fstream>
#include <algorithm>

#include <Windows.h>

struct employee
{
	int num;
	char name[10];
	double hours;
};

int main(int argc, char** argv)
{
	std::ifstream in_stream(argv[1], std::ios::binary | std::ios::in);
	std::ofstream out_stream(argv[2], std::ios::out);
	double hourly_payment = std::atof(argv[3]);

	size_t employee_count;
	in_stream.read((char*)(&employee_count), sizeof(employee_count));

	employee* employees = new employee[employee_count];

	for (size_t i = 0; i < employee_count; i++) 
	{
		in_stream.read((char*)(&employees[i]), sizeof(employees[i]));
	}
	in_stream.close();

	std::sort(employees, employees + employee_count, [](employee a, employee b) {return strcmp(a.name, b.name); });

	out_stream
		<< "Report for " << argv[1] << ":\n"
		<< "ID\tName\tHours\tSalary\n";
	for (size_t i = 0; i < employee_count; i++) 
	{
		out_stream
			<< employees[i].num << ' '
			<< employees[i].name << ' '
			<< employees[i].hours << ' '
			<< employees[i].hours * hourly_payment << '\n';
	}
	out_stream.close();
}