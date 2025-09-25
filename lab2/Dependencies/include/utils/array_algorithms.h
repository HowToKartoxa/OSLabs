#ifndef _ARRRAY_ALGORITHMS
#define _ARRAY_ALGORITHMS

#include <utils/array_data.h>

#if defined(USE_WINAPI)

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
		}
		if (array[i] < min)
		{
			Sleep(7);
			min = array[i];
		}
	}

	for (size_t i = 0; i < array.size; i++) 
	{
		if (array[i] == min) 
		{
			array.min_indices.push(i);
		}
		if (array[i] == max) 
		{
			array.max_indices.push(i);
		}
	}

	return 0;
}

DWORD WINAPI Average(LPVOID args)
{
	ArrayData& array = *reinterpret_cast<ArrayData*>(args);

	long long sum = 0;

	for (size_t i = 0; i < array.size; i++)
	{
		sum += array[i];
		Sleep(12);
	}

	array.average = sum / array.size;

	return 0;
}

#elif defined(USE_STD_THREAD)

#include <thread>
#include <chrono>

void MinMax(ArrayData* args)
{
	ArrayData& array = *args;

	size_t min = array[0];
	size_t max = array[0];

	for (size_t i = 0; i < array.size; i++)
	{
		if (array[i] > max)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(7));
			max = array[i];
		}
		if (array[i] < min)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(7));
			min = array[i];
		}
	}

	for (size_t i = 0; i < array.size; i++)
	{
		if (array[i] == min)
		{
			array.min_indices.push(i);
		}
		if (array[i] == max)
		{
			array.max_indices.push(i);
		}
	}
}

void Average(ArrayData* args)
{
	ArrayData& array = *args;

	long long sum = 0;

	for (size_t i = 0; i < array.size; i++)
	{
		sum += array[i];
		std::this_thread::sleep_for(std::chrono::milliseconds(12));
	}

	array.average = sum / array.size;
}

#endif

#endif