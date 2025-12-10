#ifndef _EMPLOYEE_DATABAE_INTERNAL
#define _EMPLOYEE_DATABASE_INTERNAL

#include <utils/employee.h>

#include <vector>
#include <algorithm>
#include <fstream>

class EmployeeDBInternal
{
	struct BTreeNode
	{
		bool is_leaf;

		size_t max_size;

		size_t& node_count;
		size_t& entry_count;

		std::vector<BTreeNode*> children;
		std::vector<size_t> keys;
		std::vector<Employee*> data;

		BTreeNode* next;

		BTreeNode(bool _is_leaf, size_t _max_size, size_t& _node_count, size_t& _entry_count) :
			is_leaf(_is_leaf),
			max_size(_max_size),
			node_count(_node_count),
			entry_count(_entry_count),
			children(),
			keys(),
			data(),
			next(nullptr)
		{
		}

		~BTreeNode()
		{
			node_count--;

			if (is_leaf)
			{
				for (auto i : data)
				{
					delete i;
				}
			}
			else
			{
				for (auto i : children)
				{
					delete i;
				}
			}
		}

		void Insert(const Employee& source)
		{
			size_t i = std::distance(keys.begin(), std::upper_bound(keys.begin(), keys.end(), source.id));
			if (i != 0 && keys[i - 1] == source.id)
			{
				return;
			}
			if (children[i]->is_leaf)
			{
				auto it = std::lower_bound(children[i]->keys.begin(), children[i]->keys.end(), source.id);
				if (it != children[i]->keys.end() && *it == source.id)
				{
					return;
				}
				entry_count++;
				children[i]->keys.insert(it, source.id);
				size_t dist = std::distance(children[i]->keys.begin(), it);
				children[i]->data.insert(children[i]->data.begin() + dist, new Employee(source));
				if (children[i]->keys.size() > max_size)
				{
					SplitChild(i, children[i]);
				}
			}
			else
			{
				children[i]->Insert(source);
				if (children[i]->keys.size() > max_size)
				{
					SplitChild(i, children[i]);
				}
			}
		}

		void SplitChild(const size_t& which, BTreeNode* child)
		{
			size_t middle = child->keys.size() / 2;
			if (child->is_leaf)
			{
				BTreeNode* new_leaf = new BTreeNode(true, max_size, node_count, entry_count);
				
				new_leaf->keys.assign(child->keys.begin() + middle, child->keys.end());
				new_leaf->data.assign(child->data.begin() + middle, child->data.end());

				child->keys.erase(child->keys.begin() + middle, child->keys.end());
				child->data.erase(child->data.begin() + middle, child->data.end());

				keys.insert(keys.begin() + which, new_leaf->keys[0]);
				children.insert(children.begin() + which + 1, new_leaf);

				new_leaf->next = child->next;
				child->next = new_leaf;
			}
			else
			{
				BTreeNode* new_node = new BTreeNode(false, max_size, node_count, entry_count);

				size_t key_val = child->keys[middle];

				new_node->keys.assign(child->keys.begin() + middle + 1, child->keys.end());
				new_node->children.assign(child->children.begin() + middle + 1, child->children.end());

				child->keys.erase(child->keys.begin() + middle, child->keys.end());
				child->children.erase(child->children.begin() + middle, child->children.end());

				keys.insert(keys.begin() + which, key_val);
				children.insert(children.begin() + which + 1, new_node);
			}

			node_count++;
		}
	}; 

	size_t node_count;
	size_t entry_count;
	size_t max_size;
	size_t max_byte_size;

	BTreeNode* root;

	size_t serialize_recursive(BTreeNode* curr, std::fstream& file, size_t& node_offset, size_t& data_offset, size_t& curr_entry_index);

	static size_t UpperBound(size_t target, size_t count, size_t* data);
	static bool BinarySearch(size_t target, size_t count, size_t* data, size_t& pos);

public:
	EmployeeDBInternal();
	~EmployeeDBInternal();

	void Insert(const Employee& source);
	size_t GetEntryCount();
	void Serialize(std::fstream& file);

	static bool Find(const size_t& id, std::fstream& file, size_t& table_index, size_t& file_pos);
	static void Get(std::fstream& file, size_t file_pos, Employee& destination);
	static void Set(std::fstream& file, size_t file_pos, Employee& source);

	static size_t GetPageSize();
};

#endif