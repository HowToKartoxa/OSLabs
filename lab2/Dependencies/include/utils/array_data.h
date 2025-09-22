#ifndef _ARRAY_DATA
#define _ARRAY_DATA

struct ArrayData 
{
	int* data;
	size_t size;
	size_t min_index;
	size_t max_index;
	int average;

	ArrayData(size_t _size) : 
		size(_size), 
		min_index(0), 
		max_index(0),
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
