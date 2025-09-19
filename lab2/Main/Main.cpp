#include <utils/array_data.h>
#include <utils/input_parsing.h>
#include <utils/array_algorithms.h>

#include <iostream>
#include <string>
#include <limits>

#include <windows.h>

#undef max()

DWORD WINAPI MinMax(LPVOID args) 
{
	ArrayData& array = *reinterpret_cast<ArrayData*>(args);

	size_t min = array[0];
	size_t max = array[0];

	for (size_t i = 0; i < array.size; i++) 
	{
		if (array[i] > max) 
		{
			Sleep(7);
			max = array[i];
			array.max_index = i;
		}
		if (array[i] < min)
		{
			Sleep(7);
			min = array[i];
			array.min_index = i;
		}
	}

	return 0;
}

DWORD WINAPI Average(LPVOID args)
{
	ArrayData& array = *reinterpret_cast<ArrayData*>(args);

	size_t sum = 0;

	for (size_t i = 0; i < array.size; i++) 
	{
		sum += array[i];
		Sleep(12);
	}

	array.average = sum / array.size;

	return 0;
}

int main(int argc, char** argv) 
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

	ArrayData array(size);
	bool invalid_data_entered = false;

	while (true) 
	{
		std::cout << "Enter array elements:\n";
		for (size_t i = 0; i < size && !invalid_data_entered; i++)
		{
			std::cin >> array[i];
			if (std::cin.fail())
			{
				std::cin.clear();
				std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
				std::cout << "Invalid data entered!\n";
				invalid_data_entered = true;
				break;
			}
		}
		if (invalid_data_entered) 
		{ 
			invalid_data_entered = false; 
		}
		else 
		{
			break;
		}
	}

	HANDLE handles[2];
	DWORD id_minmax;
	DWORD id_average;

	handles[0] = CreateThread(NULL, 0, MinMax, reinterpret_cast<LPVOID*>(&array), 0, &id_minmax);
	if (handles[0] == NULL) 
	{
		DWORD error = GetLastError();
		std::cout << "Failed to create MinMax process with code " << error << " !\n";
		std::system("pause");
		return error;
	}

	handles[1] = CreateThread(NULL, 0, Average, reinterpret_cast<LPVOID*>(&array), 0, &id_average);
	if (handles[1] == NULL) 
	{
		DWORD error = GetLastError();
		std::cout << "Failed to create Average process with code " << error << " !\n";
		std::system("pause");
		return error;
	}

	if (WaitForMultipleObjects(2, handles, TRUE, INFINITE) == WAIT_FAILED)
	{
		DWORD error = GetLastError();
		std::cout << "One of the processes failed to finish with code " << error << " !\n";
		std::system("pause");
		return error;
	}

	array[array.min_index] = array.average;
	array[array.max_index] = array.average;

	std::cout << '\n';
	for (size_t i = 0; i < size; i++) 
	{
		std::cout << array[i] << ' ';
	}
}