#include "input_parsing.h"

bool CheckFileName(std::string file_name, std::string required_postfix, bool verbose) {
    if (required_postfix != "") {
        if (file_name.length() < required_postfix.length() or
            file_name.substr(file_name.length() - required_postfix.length(), required_postfix.length()) != required_postfix) {
            if (verbose) {
                std::cout << "File name must contain " << required_postfix << " postfix!" << std::endl;
            }

            return false;
        }
    }

    if (file_name.empty()) {
        if (verbose) {
            std::cout << "File name cannot be empty!" << std::endl;
        }

        return false;
    }
    else if (file_name.length() > 255) {
        if (verbose) {
            std::cout << "File name cannot be more than 255 characters long!" << std::endl;
        }

        return false;
    }
    else {
        size_t pos = file_name.find_first_of(kForbiddenFileNameCharacters);

        if (pos != std::string::npos) {
            if (file_name[pos] == '\n' || file_name[pos] == '\t')
            {
                if (verbose) {
                    std::cout << "File name cannot contain \\t or \\n characters!" << std::endl;
                }

                return false;
            }
            else {
                if (verbose) {
                    std::cout << "File name cannot contain " << file_name[pos] << " character!" << std::endl;
                }

                return false;
            }
        }

        if (file_name.length() == 3) {
            for (int i = 0; i < 4; i++) {
                if (file_name == kForbiddenFileNames3[i]) {
                    if (verbose) {
                        std::cout << "File name " << file_name << " not allowed!" << std::endl;
                    }

                    return false;
                }
            }
        }
        else if (file_name.length() == 4) {
            for (int i = 0; i < 18; i++) {
                if (file_name == kForbiddenFileNames4[i]) {
                    if (verbose) {
                        std::cout << "File name " << file_name << " not allowed!";
                    }

                    return false;
                }
            }
        }

        if (file_name[file_name.length() - 1] == ' ' or
            file_name[file_name.length() - 1] == '.') {
            if (verbose) {
                std::cout << "File name cannot end with " << file_name[file_name.length() - 1] << " character!" << std::endl;
            }

            return false;
        }

        return true;
    }
}

bool CheckIfPositiveLong(std::string str, bool verbose) {
    std::string size_max_str = std::to_string(LONG_MAX);

    if (str.length() == 0) {
        if (verbose) {
            std::cout << "Not a number!" << std::endl;
        }

        return false;
    }
    else if (str[0] == '-') {
        if (str.length() == 1) {
            if (verbose) {
                std::cout << "Not a number!" << std::endl;
            }

            return false;
        }

        if (verbose) {
            std::cout << "Enter a positive number!" << std::endl;
        }

        return false;
    }
    else if (str.length() > size_max_str.length()) {
        if (verbose) {
            std::cout << "Enter a smaller number!" << std::endl;
        }

        return false;
    }
    else if (str.length() == size_max_str.length()) {
        for (size_t i = 0; i < str.length(); i++) {
            if (!isdigit(static_cast<unsigned char>(str[i]))) {
                if (verbose) {
                    std::cout << "Not an integer number!" << std::endl;
                }

                return false;
            }
            else if (str[i] > size_max_str[i]) {
                if (verbose) {
                    std::cout << "Enter a smaller number!" << std::endl;
                }

                return false;
            }
        }

        return true;
    }
    else {
        for (size_t i = 0; i < str.length(); i++) {
            if (!isdigit(static_cast<unsigned char>(str[i]))) {
                if (verbose) {
                    std::cout << "Not an integer number!" << std::endl;
                }

                return false;
            }
        }

        return true;
    }
}

bool CheckIfUnsignedShort(std::string str, bool verbose) {
    std::string int_max_str = std::to_string(USHRT_MAX);

    if (str.length() == 0) {
        if (verbose) {
            std::cout << "Not a number!" << std::endl;
        }

        return false;
    }
    else if (str[0] == '-') {
        if (str.length() == 1) {
            if (verbose) {
                std::cout << "Not a number!" << std::endl;
            }

            return false;
        }
        else {
            if (verbose) {
                std::cout << "Not a positive number!" << std::endl;
            }

            return false;
        }
    }

    if (str.length() > int_max_str.length()) {
        if (verbose) {
            std::cout << "Enter a smaller number!" << std::endl;
        }

        return false;
    }
    else if (str.length() == int_max_str.length()) {
        for (size_t i = 0; i < str.length(); i++) {
            if (!isdigit(static_cast<unsigned char>(str[i]))) {
                if (verbose) {
                    std::cout << "Not an integer number!" << std::endl;
                }

                return false;
            }
            else if (str[i] > int_max_str[i]) {
                if (verbose) {
                    std::cout << "Enter a smaller number!" << std::endl;
                }

                return false;
            }
        }

        return true;
    }
    else {
        for (size_t i = 0; i < str.length(); i++) {
            if (!isdigit(static_cast<unsigned char>(str[i]))) {
                if (verbose) {
                    std::cout << "Not a number!" << std::endl;
                }

                return false;
            }
        }

        return true;
    }
}