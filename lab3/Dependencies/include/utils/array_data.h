#ifndef _MARKER_PARAMETER_DATA
#define _MARKER_PARAMETER_DATA

struct MarkerParameterData 
{
	int* array_data;
	size_t array_size;
	size_t thread_index;

	int& operator[](const size_t& index) 
	{
		return array_data[index];
	}

	~MarkerParameterData() 
	{
		delete[] array_data;
	}
};

#endif