#ifndef _ARRRAY_ALGORITHMS
#define _ARRAY_ALGORITHMS

#include <windows.h>

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

#endif