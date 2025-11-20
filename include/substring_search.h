#pragma once
#include <string>
#include <vector>
#include <fstream>
#include <string_view>
#include <random>
#include <cstring>
#include <iostream>
#include <chrono>

class SubstringSearch {
public:
    void generateData(const std::string &filename, size_t dataSize, size_t patternSize);

    bool naiveSearch(const std::string &filename, std::string_view pattern) const;

    bool simdSearch(const std::string &filename, std::string_view pattern) const;
};
