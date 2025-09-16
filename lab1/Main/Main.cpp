#include "utils/input_parsing.h"
#include "utils/employee.h"

#include <iostream>
#include <string>
#include <fstream>

#include <windows.h>

int main(int argc, char** argv) 
{
	DWORD process_exit_code;

	CheckIfInt(std::to_string(INT_MIN));
	std::string binary_file_name;
	std::string employee_count_str;

	std::cout << "Enter binary file name:\n";
	std::getline(std::cin, binary_file_name);
	while (!CheckFileName(binary_file_name)) 
	{
		std::cout << "Enter binary file name:\n";
		std::getline(std::cin, binary_file_name);
	}

	std::cout << '\n';

	std::cout << "Enter employee count:\n";
	std::getline(std::cin, employee_count_str);
	while (!CheckIfSizeT(employee_count_str)) 
	{
		std::cout << "Enter employee count:\n";
		std::getline(std::cin, employee_count_str);
	}
	
	std::cout << '\n';

	std::string command_line
		= "Creator.exe "
		+ binary_file_name 
		+ ' '
		+ employee_count_str;

	STARTUPINFOA si;
	PROCESS_INFORMATION creator_process_info;

	ZeroMemory(&si, sizeof(STARTUPINFOA));
	si.cb = sizeof(STARTUPINFOA);
	
	if (!CreateProcessA(NULL, const_cast<LPSTR>(command_line.c_str()), NULL, NULL, FALSE,
		CREATE_NEW_CONSOLE, NULL, NULL, &si, &creator_process_info)) 
	{
		DWORD error = GetLastError();
		std::cout << "Failed to create process!\nError: " << error;
		system("pause");
		return 1;
	}

	WaitForSingleObject(creator_process_info.hProcess, INFINITE);

	GetExitCodeProcess(creator_process_info.hProcess, &process_exit_code);
	if (process_exit_code != 0) 
	{
		std::cout << "Creator utility failed\n";
		system("pause");
		return 1;
	}

	CloseHandle(creator_process_info.hThread);
	CloseHandle(creator_process_info.hProcess);

	size_t employee_count;

	std::ifstream binary_file(binary_file_name, std::ios::binary | std::ios::in);

	if (!binary_file.good()) 
	{
		std::cout 
			<< "Failed to open binary file" 
			<< binary_file_name 
			<< "! (That file should have been created by Creator utility.\nTry passing a different file name next time.)";
		system("pause");
		return 1;
	}

	binary_file.read((char*)(&employee_count), sizeof(employee_count));

	std::cout << "Binary file contents:\n" 
		<< employee_count << '\n';

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
	std::string hourly_payment_str;

	std::cout << "Enter report file name:\n";
	std::getline(std::cin, report_file_name);
	while (!CheckFileName(report_file_name, ".txt")) 
	{
		std::cout << "Enter report file name:\n";
		std::getline(std::cin, report_file_name);
	}

	std::cout << "Enter hourly payment ammount:\n";
	std::getline(std::cin, hourly_payment_str);
	while (!CheckIfDouble(hourly_payment_str)) 
	{
		std::cout << "Enter hourly payment ammount:\n";
		std::getline(std::cin, hourly_payment_str);
	}

	command_line
		= "Reporter.exe "
		+ binary_file_name + ' '
		+ report_file_name + ' '
		+ hourly_payment_str;

	ZeroMemory(&si, sizeof(STARTUPINFOA));
	si.cb = sizeof(STARTUPINFOA);

	if (!CreateProcessA(NULL, (LPSTR)command_line.c_str(), NULL, NULL, FALSE,
		CREATE_NEW_CONSOLE, NULL, NULL, &si, &creator_process_info)) 
	{
		DWORD error = GetLastError();
		std::cout << "Failed to create process!\nError: " << error;
		system("pause");
		return 1;
	}

	WaitForSingleObject(creator_process_info.hProcess, INFINITE);

	GetExitCodeProcess(creator_process_info.hProcess, &process_exit_code);
	if (process_exit_code != 0)
	{
		std::cout << "Reporter utility failed\n";
		system("pause");
		return 1;
	}

	CloseHandle(creator_process_info.hThread);
	CloseHandle(creator_process_info.hProcess);

	std::ifstream report_file(report_file_name, std::ios::in);

	if (!report_file.good())
	{
		std::cout
			<< "Failed to open text file"
			<< binary_file_name
			<< "! (That file should have been created by Reporter utility.\nTry passing a different file name next time.)";
		system("pause");
		return 1;
	}

	std::cout << "Report file contents:\n";

	char temp;
	while (report_file >> std::noskipws >> temp) 
	{
		std::cout << temp;
	}

	system("pause");

	return 0;
}