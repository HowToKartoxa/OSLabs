#include <utils/employee_database_internal.h>

#include <fstream>
#include <string.h>

#if defined(USE_WINAPI)

	#include <windows.h>

#elif defined(USE_BOOST)

	#include <boost/interprocess/mapped_region.hpp>

#endif

EmployeeDBInternal::EmployeeDBInternal() : node_count(0), entry_count(0)
{
	max_byte_size = GetPageSize();

	max_size = (max_byte_size - sizeof(char) - sizeof(size_t)) / sizeof(size_t) / 2 - 1;

	root = new BTreeNode(true, max_size, node_count, entry_count);
	node_count++;
}

EmployeeDBInternal::~EmployeeDBInternal()
{
	delete root;
}

void EmployeeDBInternal::Insert(const Employee& source)
{
	if (root->is_leaf)
	{
		auto it = std::lower_bound(root->keys.begin(), root->keys.end(), source.id);
		if (it != root->keys.end() && *it == source.id)
		{
			return;
		}
		entry_count++;
		size_t dist = std::distance(root->keys.begin(), it);
		root->keys.insert(it, source.id);
		root->data.insert(root->data.begin() + dist, new Employee(source));
		if (root->keys.size() > max_size)
		{
			BTreeNode* new_root = new BTreeNode(false, max_size, node_count, entry_count);
			new_root->children.push_back(root);
			new_root->SplitChild(0, root);
			root = new_root;
			
			node_count++;
		}
	}
	else
	{
		root->Insert(source);
		if (root->keys.size() > max_size)
		{
			BTreeNode* new_root = new BTreeNode(false, max_size, node_count, entry_count);
			new_root->children.push_back(root);
			new_root->SplitChild(0, root);
			root = new_root;

			node_count++;
		}
	}
}

size_t EmployeeDBInternal::GetEntryCount()
{
	return entry_count;
}

size_t EmployeeDBInternal::UpperBound(size_t target, size_t count, size_t* data)
{
	size_t left = 0;
	size_t right = count;
	size_t middle;
	while (left < right)
	{
		middle = (right - left) / 2ull + left;
		if (target >= data[middle]) left = middle + 1ull;
		else right = middle;
	}
	return left;
}

bool EmployeeDBInternal::BinarySearch(size_t target, size_t count, size_t* data, size_t& pos)
{
	size_t left = 0;
	size_t right = count;
	size_t middle;
	while (left < right)
	{
		middle = (right - left) / 2ull + left;
		if (target == data[middle])
		{
			pos = middle;
			return true;
		}
		else if (target > data[middle]) left = middle + 1;
		else right = middle;
	}
	return false;
}

size_t EmployeeDBInternal::serialize_recursive(BTreeNode* curr, std::fstream& file, size_t& node_offset, size_t& data_offset, size_t& curr_entry_index)
{
	file.seekp(node_offset);
	size_t temp, temp_p = node_offset;
	node_offset += max_byte_size;

	if (curr->is_leaf)
	{
		char type = '\1';
		size_t size = curr->keys.size();
		file.write(&type, sizeof(char));
		file.write(reinterpret_cast<char*>(&size), sizeof(size_t));
		temp = data_offset;

		for (size_t i = 0; i < curr->keys.size(); i++)
		{
			file.write(reinterpret_cast<char*>(&curr->keys[i]), sizeof(size_t));
		}
		
		for (size_t i = 0; i < curr->data.size(); i++)
		{
			file.write(reinterpret_cast<char*>(&temp), sizeof(size_t));
			temp += sizeof(Employee) + sizeof(size_t);
		}

		file.seekp(data_offset);
		for (size_t i = 0; i < curr->data.size(); i++)
		{
			file.write(reinterpret_cast<char*>(curr->data[i]), sizeof(Employee));
			file.write(reinterpret_cast<char*>(&curr_entry_index), sizeof(size_t));
			curr_entry_index++;
			data_offset += sizeof(Employee) + sizeof(size_t);
		}

		return temp_p;
	}
	else
	{
		char type = '\0';
		size_t size = curr->keys.size();
		file.write(&type, sizeof(char));
		file.write(reinterpret_cast<char*>(&size), sizeof(size_t));

		for (size_t i = 0; i < curr->keys.size(); i++)
		{
			file.write(reinterpret_cast<char*>(&curr->keys[i]), sizeof(size_t));
		}
		for (size_t i = 0; i < curr->children.size(); i++)
		{
			temp = file.tellp();
			temp_p = serialize_recursive(curr->children[i], file, node_offset, data_offset, curr_entry_index);
			file.seekp(temp);
			file.write(reinterpret_cast<char*>(&temp_p), sizeof(size_t));
		}

		return temp_p;
	}
}

void EmployeeDBInternal::Serialize(std::fstream& file)
{
	size_t node_offset = 0, data_offset = node_count * max_byte_size, curr_entry_index = 0;
	serialize_recursive(root, file, node_offset, data_offset, curr_entry_index);
}

size_t EmployeeDBInternal::GetPageSize()
{
#if defined(USE_WINAPI)

	SYSTEM_INFO info;
	GetSystemInfo(&info);
	return info.dwPageSize;

#elif defined(USE_BOOST)

	return boost::interprocess::mapped_region::get_page_size();

#endif
}

bool EmployeeDBInternal::Find(const size_t& id, std::fstream& file, size_t& table_index, size_t& file_pos)
{
	file_pos = 0;
	size_t node_size = GetPageSize();
	char* buffer = new char[node_size];
	char type;
	size_t size;
	size_t* temp_arr;

	while (true)
	{
		file.seekg(file_pos);
		file.read(buffer, node_size);
		type = buffer[0];
		memcpy_s(&size, sizeof(size_t), &buffer[1], sizeof(size_t));
		temp_arr = reinterpret_cast<size_t*>(buffer + sizeof(char) + sizeof(size_t));
		if (type == '\1')
		{
			if (BinarySearch(id, size, temp_arr, file_pos))
			{
				file_pos = (temp_arr + size)[file_pos];
				file.seekg(file_pos + sizeof(Employee));
				file.read(reinterpret_cast<char*>(&table_index), sizeof(size_t));
				
				return true;
			}
			else return false;
		}
		else
		{
			file_pos = UpperBound(id, size, temp_arr);
			file_pos = (temp_arr + size)[file_pos];
		}
	}
}

void EmployeeDBInternal::Get(std::fstream& file, size_t file_pos, Employee& destination)
{
	file.seekg(file_pos);
	file.read(reinterpret_cast<char*>(&destination), sizeof(Employee));
}

void EmployeeDBInternal::Set(std::fstream& file, size_t file_pos, Employee& source)
{
	file.seekp(file_pos);
	file.write(reinterpret_cast<char*>(&source), sizeof(Employee));
}