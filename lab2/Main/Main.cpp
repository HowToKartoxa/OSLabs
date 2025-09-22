#if not defined(USE_WINAPI) && not defined(USE_STD_THREAD)

#define USE_WINAPI

#endif

#include <utils/array_data.h>
#include <utils/input_parsing.h>
#include <utils/array_algorithms.h>

#include <iostream>
#include <string>
#include <limits>

#if defined (USE_WINAPI)

#include <windows.h>

#elif defined (USE_STD_THREAD)

#include <thread>

#endif

#undef max()

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

#if defined (USE_WINAPI)

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
		std::cout << "Processes failed to finish with code " << error << " !\n";
		std::system("pause");
		return error;
	}

#elif defined (USE_STD_THREAD)

	try 
	{
		std::thread min_max_thread(MinMax, &array);
		min_max_thread.join();
	}
	catch (const std::system_error e) 
	{
		std::cout << "Failed to create MinMax process with error\n" << e.what();
		std::system("pause");
		return 1;
	}

	try
	{
		std::thread average_thread(Average, &array);
		average_thread.join();
	}
	catch (const std::system_error e)
	{
		std::cout << "Failed to create Average process with error\n" << e.what();
		std::system("pause");
		return 1;
	}

#endif

	array[array.min_index] = array.average;
	array[array.max_index] = array.average;

	std::cout << '\n';
	for (size_t i = 0; i < size; i++) 
	{
		std::cout << array[i] << ' ';
	}
}