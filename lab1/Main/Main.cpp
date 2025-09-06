#include <iostream>
#include <string>
#include <fstream>

#include <windows.h>
#include <stringapiset.h>

struct employee
{
	int num;
	char name[10];
	double hours;
};

int main(int argc, char** argv) 
{
	size_t number_of_chars_converted;

	std::string binary_file_name;
	size_t employee_count;

	std::cout << "Enter binary file name: ";
	std::getline(std::cin, binary_file_name);
	std::cout << "\nEnter employee count: ";
	std::cin >> employee_count;

	std::string command_line
		= "Creator.exe "
		+ binary_file_name + ' '
		+ std::to_string(employee_count);

	STARTUPINFOA si;
	PROCESS_INFORMATION creator_process_info;

	ZeroMemory(&si, sizeof(STARTUPINFOA));
	si.cb = sizeof(STARTUPINFOA);
	
	CreateProcessA(NULL, (LPSTR)command_line.c_str(), NULL, NULL, FALSE,
		CREATE_NEW_CONSOLE, NULL, NULL, &si, &creator_process_info);

	WaitForSingleObject(creator_process_info.hProcess, INFINITE);
	CloseHandle(creator_process_info.hThread);
	CloseHandle(creator_process_info.hProcess);

	std::ifstream binary_file(binary_file_name, std::ios::binary | std::ios::in);
	binary_file.read((char*)(&employee_count), sizeof(employee_count));
	std::cout << employee_count << '\n';
	employee temp_employee;
	for (size_t i = 0; i < employee_count; i++) 
	{
		binary_file.read((char*)(&temp_employee), sizeof(temp_employee));
		std::cout
			<< temp_employee.num << ' '
			<< temp_employee.name << ' '
			<< temp_employee.hours << '\n';
	}
	binary_file.close();

	std::string report_file_name;
	float hourly_payment;

	std::cout << "Enter report file name: ";
	std::cin.ignore();
	std::getline(std::cin, report_file_name);
	std::cout << "Enter hourly payment ammount: ";
	std::cin >> hourly_payment;

	command_line
		= "Reporter.exe "
		+ binary_file_name + ' '
		+ report_file_name + ' '
		+ std::to_string(hourly_payment);

	ZeroMemory(&si, sizeof(STARTUPINFOA));
	si.cb = sizeof(STARTUPINFOA);

	CreateProcessA(NULL, (LPSTR)command_line.c_str(), NULL, NULL, FALSE,
		CREATE_NEW_CONSOLE, NULL, NULL, &si, &creator_process_info);

	WaitForSingleObject(creator_process_info.hProcess, INFINITE);
	CloseHandle(creator_process_info.hThread);
	CloseHandle(creator_process_info.hProcess);

	std::ifstream report_file(report_file_name, std::ios::in);
	char temp;
	while (report_file >> std::noskipws >> temp) 
	{
		std::cout << temp;
	}

	system("pause");

	return 0;
}