#include <utils/employee_database_internal.h>

#include <fstream>
#include <string.h>

#define USE_WINAPI

#if defined(USE_WINAPI)

	#include <windows.h>

#elif defined(USE_BOOST)

	#include <boost/interprocess/mapped_region.hpp>

#endif

EmployeeDBInternal::EmployeeDBInternal(const std::vector<Employee>& data) : node_count(0), entry_count(0)
{
	max_byte_size = GetPageSize();

	max_size = ((max_byte_size - sizeof(char)) / sizeof(size_t) - 2) / 2;

	root = new BTreeNode(true, max_size, node_count);

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
		root->keys.insert(it, source.id);
		size_t dist = std::distance(root->keys.begin(), it);
		root->data.insert(root->data.begin() + dist, new Employee(source));
		if (root->keys.size() > max_size)
		{
			BTreeNode* new_root = new BTreeNode(false, max_size, node_count);
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
			BTreeNode* new_root = new BTreeNode(false, max_size, node_count);
			new_root->children.push_back(root);
			new_root->SplitChild(0, root);
			root = new_root;

			node_count++;
		}
	}
}

size_t EmployeeDBInternal::serialize_recursive(BTreeNode* curr, std::fstream& file, size_t& node_offset, size_t& data_offset)
{
	file.seekp(node_offset);
	size_t temp, temp_p = node_offset;
	node_offset += max_size;

	if (curr->is_leaf)
	{
		NodeInfo info('\1', curr->keys.size());
		file.write(reinterpret_cast<char*>(&info), sizeof(NodeInfo));
		temp = data_offset;

		for (size_t i = 0; i < curr->keys.size(); i++)
		{
			file.write(reinterpret_cast<char*>(&curr->keys[i]), sizeof(size_t));
		}
		for (size_t i = 0; i < curr->data.size(); i++)
		{
			file.write(reinterpret_cast<char*>(&temp), sizeof(size_t));
			temp += sizeof(size_t);
		}

		file.seekp(data_offset);
		for (size_t i = 0; i < curr->data.size(); i++)
		{
			file.write(reinterpret_cast<char*>(curr->data[i]), sizeof(Employee));
			data_offset += sizeof(Employee);
		}

		return temp_p;
	}
	else
	{
		NodeInfo info('\0', curr->keys.size());
		file.write(reinterpret_cast<char*>(&info), sizeof(NodeInfo));

		for (size_t i = 0; i < curr->keys.size(); i++)
		{
			file.write(reinterpret_cast<char*>(&curr->keys[i]), sizeof(size_t));
		}
		for (size_t i = 0; i < curr->children.size(); i++)
		{
			temp_p = file.tellp();
			temp = serialize_recursive(curr->children[i], file, node_offset, data_offset);
			file.seekp(temp_p);
			file.write(reinterpret_cast<char*>(&temp), sizeof(size_t));
		}
	}
}

void EmployeeDBInternal::Serialize(std::fstream& file)
{
	size_t node_offset = 0, data_offset = node_count * max_byte_size;
	serialize_recursive(root, file, node_offset, data_offset);
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

bool EmployeeDBInternal::Find(const size_t& id, std::fstream file, size_t& file_pos)
{
	file_pos = 0;
	size_t node_size = GetPageSize();
	char* buffer = new char[node_size];
	NodeInfo info('\1', 0);
	size_t* temp_arr;

	while (true)
	{
		file.seekg(file_pos);
		file.read(buffer, node_size);
		memcpy_s(&info, sizeof(NodeInfo), &buffer, sizeof(NodeInfo));
		temp_arr = reinterpret_cast<size_t*>(buffer + sizeof(NodeInfo));
		if (info.type == '\1')
		{
			if (BinarySearch(id, info.size, temp_arr, file_pos))
			{
				file_pos = (temp_arr + info.size)[file_pos];
				return true;
			}
			else return false;
		}
		else
		{
			file_pos = UpperBound(id, info.size, temp_arr);
			file_pos = (temp_arr + info.size)[file_pos];
		}
	}
}

void EmployeeDBInternal::Get(std::fstream file, size_t file_pos, Employee& destination)
{
	file.seekg(file_pos);
	file.read(reinterpret_cast<char*>(&destination), sizeof(Employee));
}

void EmployeeDBInternal::Set(std::fstream file, size_t file_pos, Employee& source)
{
	file.seekp(file_pos);
	file.write(reinterpret_cast<char*>(&source), sizeof(Employee));
}