#ifndef _ARRAY_DATA
#define _ARRAY_DATA

#include <stack>

struct ArrayData 
{
	int* data;
	size_t size;
	std::stack<size_t> min_indices;
	std::stack<size_t> max_indices;
	int average;

	ArrayData(size_t _size) : 
		size(_size), 
		min_indices(),
		max_indices(),
		average()
	{
		data = new int[size];
	}

	int& operator[](const size_t& index) 
	{
		return data[index];
	}

	~ArrayData() 
	{
		delete data;
	}
};

#endif
