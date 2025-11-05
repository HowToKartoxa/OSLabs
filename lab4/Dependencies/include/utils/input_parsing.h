#ifndef INPUT_PARSING_H_
#define INPUT_PARSING_H_

#include <string>

const char kForbiddenFileNameCharacters[12] = "<>|\\/:\"?*\t\n";
const char kForbiddenFileNames3[4][4] = { "CON", "PRN", "AUX", "NUL" };
const char kForbiddenFileNames4[18][5] = { "COM1", "COM2", "COM3", "COM4", "COM5", "COM6", "COM7", "COM8", "COM9", "LPT1", "LPT2", "LPT3", "LPT4", "LPT5", "LPT6", "LPT7", "LPT8", "LPT9" };

bool CheckFileName(std::string, std::string, bool verbose = true);
bool CheckIfSizeT(std::string, bool verbose = true);
bool CheckIfPositiveLong(std::string, bool verbose = true);
bool CheckIfInt(std::string, bool verbose = true);
bool CheckIfUnsignedShort(std::string, bool verbose = true);
bool CheckIfDouble(std::string, bool verbose = true);

size_t StringToSizeT(std::string);

#endif