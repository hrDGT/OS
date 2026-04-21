#include <string>
#include <cstddef>
#include <iostream>
#include <stdexcept>
#include <cctype>
#include <climits>

constexpr const char* kForbiddenFileNameCharacters = "\\/:*?\"<>|";
constexpr const char* kForbiddenFileNames3[] = { "CON", "PRN", "AUX", "NUL" };
constexpr const char* kForbiddenFileNames4[] = { "COM1","COM2","COM3","COM4","COM5","COM6","COM7","COM8","COM9",
                                               "LPT1","LPT2","LPT3","LPT4","LPT5","LPT6","LPT7","LPT8","LPT9" };

bool CheckFileName(std::string file_name, std::string required_postfix = "", bool verbose = false);
bool CheckIfPositiveLong(std::string str, bool verbose = false);
bool CheckIfUnsignedShort(std::string str, bool verbose = false);