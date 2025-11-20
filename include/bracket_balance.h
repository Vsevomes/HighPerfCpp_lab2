#pragma once
#include <string>
#include <vector>
#include <fstream>
#include <random>
#include <chrono>
#include <stdexcept>

class BracketBalance {
public:
    void generateData(const std::string &filename, size_t dataSize);

    bool naiveCheck(const std::string &filename) const;

    bool simdCheck(const std::string &filename) const;
};