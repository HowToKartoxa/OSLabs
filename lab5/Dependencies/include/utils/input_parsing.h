#ifndef INPUT_PARSING_H_
#define INPUT_PARSING_H_

#include <string>
#include <iostream>

const char kForbiddenFileNameCharacters[12] = "<>|\\/:\"?*\t\n";
const char kForbiddenFileNames3[4][4] = { "CON", "PRN", "AUX", "NUL" };
const char kForbiddenFileNames4[18][5] = { "COM1", "COM2", "COM3", "COM4", "COM5", "COM6", "COM7", "COM8", "COM9", "LPT1", "LPT2", "LPT3", "LPT4", "LPT5", "LPT6", "LPT7", "LPT8", "LPT9" };

bool CheckFileName(std::string, std::string, bool verbose = true);
bool CheckIfSizeT(std::string, bool verbose = true);
bool CheckIfPositiveLong(std::string, bool verbose = true);
bool CheckIfInt(std::string, bool verbose = true);
bool CheckIfUnsignedInt(std::string, bool verbose = true);
bool CheckIfUnsignedShort(std::string, bool verbose = true);
bool CheckIfDouble(std::string, bool verbose = true);

size_t StringToSizeT(std::string);

template <typename T>
T Query(std::string message);

template<>
inline unsigned int Query<unsigned int>(std::string message)
{
	std::string temp;
	std::cout << message << '\n';
	std::getline(std::cin, temp);
	while (!CheckIfUnsignedInt(temp))
	{
		std::cout << message << '\n';
		std::getline(std::cin, temp);
	}
	return atoi(temp.c_str());
}

template<>
inline unsigned short Query<unsigned short>(std::string message)
{
	std::string temp;
	std::cout << message << '\n';
	std::getline(std::cin, temp);
	while (!CheckIfUnsignedShort(temp))
	{
		std::cout << message << '\n';
		std::getline(std::cin, temp);
	}
	return atoi(temp.c_str());
}

template<>
inline double Query<double>(std::string message)
{
	std::string temp;
	std::cout << message << '\n';
	std::getline(std::cin, temp);
	while (!CheckIfDouble(temp))
	{
		std::cout << message << '\n';
		std::getline(std::cin, temp);
	}
	return std::stod(temp);
}

template<>
inline std::string Query<std::string>(std::string message)
{
	std::string temp;
	std::cout << message << '\n';
	std::getline(std::cin, temp);
	return temp;
}

template<>
inline size_t Query<size_t>(std::string message)
{
	std::string temp;
	std::cout << message << '\n';
	std::getline(std::cin, temp);
	while (!CheckIfSizeT(temp))
	{
		std::cout << message << '\n';
		std::getline(std::cin, temp);
	}
	return StringToSizeT(temp);
}

inline std::string QueryFileName(std::string message, std::string postfix)
{
	std::string temp;
	std::cout << message << '\n';
	std::getline(std::cin, temp);
	while (!CheckFileName(temp, postfix))
	{
		std::cout << message << '\n';
		std::getline(std::cin, temp);
	}
	return temp;
}

#endif