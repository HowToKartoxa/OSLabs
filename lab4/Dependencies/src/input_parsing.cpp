#include <utils/input_parsing.h>

#include <iostream>
#include <stdexcept>
#include <cctype>

bool CheckFileName(std::string file_name, std::string required_postfix = "", bool verbose)
{
	if (required_postfix != "")
	{
		if (file_name.length() < required_postfix.length() ||
			file_name.substr(file_name.length() - required_postfix.length(), required_postfix.length()) != required_postfix)
		{
			if (verbose) std::cout << "File name must contain " << required_postfix << " postfix!\n";
			return false;
		}
	}
	if (file_name.empty())
	{
		if (verbose) std::cout << "File name cannot be empty!\n";
		return false;
	}
	else if (file_name.length() > 255)
	{
		if (verbose) std::cout << "File name cannot be more than 255 characters long!\n";
		return false;
	}
	else if (size_t temp_index = file_name.find_first_of(kForbiddenFileNameCharacters) != std::string::npos)
	{
		if (file_name[temp_index] == '\n' || file_name[temp_index] == '\t')
		{
			if (verbose) std::cout << "File name cannot contain \\t or \\n charachters!\n";
			return false;
		}
		else
		{
			if (verbose) std::cout << "File name cannot contain " << file_name[temp_index] << " character!\n";
			return false;
		}
	}
	else
	{
		if (file_name.length() == 3)
		{
			for (int i = 0; i < 4; i++)
			{
				if (file_name == kForbiddenFileNames3[i])
				{
					if (verbose) std::cout << "File name " << file_name << " not allowed!\n";
					return false;
				}
			}
		}
		else if (file_name.length() == 4)
		{
			for (int i = 0; i < 18; i++)
			{
				if (file_name == kForbiddenFileNames4[i])
				{
					if (verbose) std::cout << "File name " << file_name << " not allowed!\n";
					return false;
				}
			}
		}
		if (file_name[file_name.length() - 1] == ' ' || file_name[file_name.length() - 1] == '.')
		{
			if (verbose) std::cout << "File name cannot end with " << file_name[file_name.length() - 1] << " character!\n";
			return false;
		}
		else
		{
			return true;
		}
	}
}

bool CheckIfSizeT(std::string str, bool verbose)
{
	std::string size_max_str = std::to_string(SIZE_MAX);
	if (str.length() == 0)
	{
		if (verbose) std::cout << "Not a number!\n";
		return false;
	}
	else if (str[0] == '-')
	{
		if (str.length() == 1)
		{
			if (verbose) std::cout << "Not a number!\n";
			return false;
		}
		if (verbose) std::cout << "Enter a positive number!\n";
		return false;
	}
	else if (str.length() > size_max_str.length())
	{
		if (verbose) std::cout << "Enter a smaller number!\n";
		return false;
	}
	else if (str.length() == size_max_str.length())
	{
		for (size_t i = 0; i < str.length(); i++)
		{
			if (!isdigit(str[i]))
			{
				if (verbose) std::cout << "Not an integer number!\n";
				return false;
			}
			else if (str[i] > size_max_str[i])
			{
				if (verbose) std::cout << "Enter a smaller number!\n";
				return false;
			}
		}
		return true;
	}
	else
	{
		for (size_t i = 0; i < str.length(); i++)
		{
			if (!isdigit(str[i]))
			{
				if (verbose) std::cout << "Not an integer number!\n";
				return false;
			}
		}
		return true;
	}
}

bool CheckIfPositiveLong(std::string str, bool verbose)
{
	std::string size_max_str = std::to_string(LONG_MAX);
	if (str.length() == 0)
	{
		if (verbose) std::cout << "Not a number!\n";
		return false;
	}
	else if (str[0] == '-')
	{
		if (str.length() == 1)
		{
			if (verbose) std::cout << "Not a number!\n";
			return false;
		}
		if (verbose) std::cout << "Enter a positive number!\n";
		return false;
	}
	else if (str.length() > size_max_str.length())
	{
		if (verbose) std::cout << "Enter a smaller number!\n";
		return false;
	}
	else if (str.length() == size_max_str.length())
	{
		for (size_t i = 0; i < str.length(); i++)
		{
			if (!isdigit(str[i]))
			{
				if (verbose) std::cout << "Not an integer number!\n";
				return false;
			}
			else if (str[i] > size_max_str[i])
			{
				if (verbose) std::cout << "Enter a smaller number!\n";
				return false;
			}
		}
		return true;
	}
	else
	{
		for (size_t i = 0; i < str.length(); i++)
		{
			if (!isdigit(str[i]))
			{
				if (verbose) std::cout << "Not an integer number!\n";
				return false;
			}
		}
		return true;
	}
}

bool CheckIfInt(std::string str, bool verbose)
{
	std::string int_max_str = std::to_string(INT_MAX);
	bool is_negative = false;
	if (str.length() == 0)
	{
		if (verbose) std::cout << "Not a number!\n";
		return false;
	}
	else if (str[0] == '-')
	{
		if (str.length() == 1)
		{
			if (verbose) std::cout << "Not a number!\n";
			return false;
		}
		str.erase(0, 1);
		is_negative = true;
	}
	if (str.length() > int_max_str.length())
	{
		if (verbose) std::cout << "Enter a smaller number!\n";
		return false;
	}
	else if (str.length() == int_max_str.length())
	{
		for (size_t i = 0; i < str.length(); i++)
		{
			if (!isdigit(str[i]))
			{
				if (verbose) std::cout << "Not an integer number!\n";
				return false;
			}
			else if (str[i] > int_max_str[i] && !(i == int_max_str.length() - 1 && is_negative))
			{
				if (verbose) std::cout << "Enter a smaller number!\n";
				return false;
			}
		}
		return true;
	}
	else
	{
		for (size_t i = 0; i < str.length(); i++)
		{
			if (!isdigit(str[i]))
			{
				if (verbose) std::cout << "Not a number!\n";
				return false;
			}
		}
		return true;
	}
}

bool CheckIfUnsignedShort(std::string str, bool verbose)
{
	std::string int_max_str = std::to_string(USHRT_MAX);
	bool is_negative = false;
	if (str.length() == 0)
	{
		if (verbose) std::cout << "Not a number!\n";
		return false;
	}
	else if (str[0] == '-')
	{
		if (str.length() == 1)
		{
			if (verbose) std::cout << "Not a number!\n";
			return false;
		}
		else
		{
			if (verbose) std::cout << "Not a positive number!\n";
			return false;
		}
	}
	if (str.length() > int_max_str.length())
	{
		if (verbose) std::cout << "Enter a smaller number!\n";
		return false;
	}
	else if (str.length() == int_max_str.length())
	{
		for (size_t i = 0; i < str.length(); i++)
		{
			if (!isdigit(str[i]))
			{
				if (verbose) std::cout << "Not an integer number!\n";
				return false;
			}
			else if (str[i] > int_max_str[i] && !(i == int_max_str.length() - 1 && is_negative))
			{
				if (verbose) std::cout << "Enter a smaller number!\n";
				return false;
			}
		}
		return true;
	}
	else
	{
		for (size_t i = 0; i < str.length(); i++)
		{
			if (!isdigit(str[i]))
			{
				if (verbose) std::cout << "Not a number!\n";
				return false;
			}
		}
		return true;
	}
}

size_t StringToSizeT(std::string str)
{
	size_t result = 0;
	for (int i = 0; i < str.length(); i++)
	{
		result *= 10;
		result += str[i] - '0';
	}
	return result;
}

bool CheckIfDouble(std::string str, bool verbose)
{
	if (str.empty())
	{
		if (verbose) std::cout << "Not a number!\n";
		return false;
	}
	else if (str[0] == '-' && str.length() == 1)
	{
		if (verbose) std::cout << "Not a number!\n";
		return false;
	}
	try
	{
		std::stod(str);
	}
	catch (...)
	{
		if (verbose) std::cout << "Not a double (extended floating point) number!\n";
		return false;
	}
	return true;
}