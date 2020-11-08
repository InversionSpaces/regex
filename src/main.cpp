#include <iostream>

#include "regex.hpp"

using namespace regex;

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cout << "USAGE: " << argv[0] << " REGEX STR" << std::endl;
        return 0;
    }

    std::cout << Regex::large_acceptable_substring(argv[1], argv[2]) << std::endl;
}
