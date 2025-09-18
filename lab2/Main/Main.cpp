#include <utils/ArrayData.h>

#include <iostream>

#include <windows.h>

DWORD WINAPI MinMax(LPVOID args) 
{
	ArrayData* array = reinterpret_cast<ArrayData*>(args);

	size_t min = array->data[0];
	size_t max = array->data[0];

	for (size_t i = 0; i < array->size; i++) 
	{
		if (array->data[i] > max) 
		{
			Sleep(7);
			max = array->data[i];
			array->max_index = i;
		}
		if (array->data[i] < min)
		{
			Sleep(7);
			min = array->data[i];
			array->min_index = i;
		}
	}

	return 0;
}

DWORD WINAPI Average(LPVOID args)
{
	ArrayData* array = reinterpret_cast<ArrayData*>(args);

	size_t sum = 0;

	for (size_t i = 0; i < array->size; i++) 
	{
		sum += array->data[i];
		Sleep(12);
	}

	array->average = sum / array->size;

	return 0;
}

int main(int argc, char** argv) 
{
	size_t size;
	std::cout << "Enter array size:\n";
	std::cin >> size;

	ArrayData array(size);
	std::cout << "Enter array elements:\n";
	for (size_t i = 0; i < size; i++) 
	{
		std::cin >> array.data[i];
	}

	HANDLE handles[2];
	DWORD id_minmax;
	DWORD id_average;

	handles[0] = CreateThread(NULL, 0, MinMax, reinterpret_cast<LPVOID*>(&array), 0, &id_minmax);
	if (handles[0] == NULL)
		return GetLastError();
	handles[1] = CreateThread(NULL, 0, Average, reinterpret_cast<LPVOID*>(&array), 0, &id_average);
	if (handles[1] == NULL)
		return GetLastError();

	if (WaitForMultipleObjects(2, handles, TRUE, INFINITE) == WAIT_FAILED)
	{
		return GetLastError();
	}

	array.data[array.min_index] = array.average;
	array.data[array.max_index] = array.average;

	std::cout << '\n';
	for (size_t i = 0; i < size; i++) 
	{
		std::cout << array.data[i] << ' ';
	}
}