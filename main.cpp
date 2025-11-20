// #include <iostream>
// #include <chrono>
// #include <filesystem>
// #include "substring_search.h"
// #include "bracket_balance.h"

// #define MB 1024*1024

// int main() {
//     std::filesystem::create_directory("../data");
//     const std::string substringFile = "../data/data_substring.txt";
//     const size_t substringSize = 10 * MB; 
//     const size_t patternSize = 8;

//     SubstringSearch ss;
//     ss.generateData(substringFile, substringSize, patternSize);

//     // Load pattern for search
//     std::ifstream pf("../data/pattern.txt", std::ios::binary);
//     std::string pattern(patternSize, ' ');
//     pf.read(&pattern[0], patternSize);

//     std::cout << "=== Substring Search ===\n";
//     auto start = std::chrono::high_resolution_clock::now();
//     bool naiveFound = ss.naiveSearch(substringFile, pattern);
//     auto end = std::chrono::high_resolution_clock::now();
//     double duration = std::chrono::duration<double>(end - start).count();
//     std::cout << "Naive found: " << naiveFound << ", time: " << duration << " s, speed: "
//               << static_cast<size_t>(substringSize / duration) << " B/s\n";

//     start = std::chrono::high_resolution_clock::now();
//     bool simdFound = ss.simdSearch(substringFile, pattern);
//     end = std::chrono::high_resolution_clock::now();
//     duration = std::chrono::duration<double>(end - start).count();
//     std::cout << "SIMD found: " << simdFound << ", time: " << duration << " s, speed: "
//               << static_cast<size_t>(substringSize / duration) << " B/s\n";


//     const std::string bracketsFile = "../data/data_brackets.txt";
//     const size_t bracketsSize = 10 * MB; 

//     BracketBalance bb;
//     bb.generateData(bracketsFile, bracketsSize);

//     std::cout << "=== Bracket Balance ===\n";
//     start = std::chrono::high_resolution_clock::now();
//     bool naiveBalanced = bb.naiveCheck(bracketsFile);
//     end = std::chrono::high_resolution_clock::now();
//     duration = std::chrono::duration<double>(end - start).count();
//     std::cout << "Naive balanced: " << naiveBalanced << ", time: " << duration << " s, speed: "
//               << static_cast<size_t>(bracketsSize / duration) << " B/s\n";

//     start = std::chrono::high_resolution_clock::now();
//     bool simdBalanced = bb.simdCheck(bracketsFile);
//     end = std::chrono::high_resolution_clock::now();
//     duration = std::chrono::duration<double>(end - start).count();
//     std::cout << "SIMD balanced: " << simdBalanced << ", time: " << duration << " s, speed: "
//               << static_cast<size_t>(bracketsSize / duration) << " B/s\n";

//     return 0;
// }

#include <benchmark/benchmark.h>
#include <filesystem>
#include "substring_search.h"
#include "bracket_balance.h"

#define MB (1024*1024)

std::vector<size_t> sizes = {1*MB, 10*MB, 100*MB, 500*MB};

SubstringSearch ss;
std::vector<std::string> substringFiles;

BracketBalance bb;
std::vector<std::string> bracketFiles;

void prepareFiles() {
    std::filesystem::create_directory("../data");

    substringFiles.clear();
    bracketFiles.clear();

    for (size_t sz : sizes) {
        std::string sfile = "../data/data_substring_" + std::to_string(sz/MB) + "MB.txt";
        ss.generateData(sfile, sz, 8);
        substringFiles.push_back(sfile);

        std::string bfile = "../data/data_brackets_" + std::to_string(sz/MB) + "MB.txt";
        bb.generateData(bfile, sz);
        bracketFiles.push_back(bfile);
    }
}

// Google Benchmark для substring naive
static void BM_NaiveSubstring(benchmark::State& state) {
    size_t idx = state.range(0);
    const std::string &file = substringFiles[idx];
    std::string pattern(8, ' '); // фиксированный паттерн

    size_t sz = sizes[idx];

    for (auto _ : state) {
        bool found = ss.naiveSearch(file, pattern);
        benchmark::DoNotOptimize(found);
    }
    state.SetBytesProcessed(int64_t(state.iterations()) * sz);
}

// Google Benchmark для substring SIMD
static void BM_SIMDSubstring(benchmark::State& state) {
    size_t idx = state.range(0);
    const std::string &file = substringFiles[idx];
    std::string pattern(8, ' ');

    size_t sz = sizes[idx];

    for (auto _ : state) {
        bool found = ss.simdSearch(file, pattern);
        benchmark::DoNotOptimize(found);
    }
    state.SetBytesProcessed(int64_t(state.iterations()) * sz);
}

// Google Benchmark для bracket naive
static void BM_NaiveBracket(benchmark::State& state) {
    size_t idx = state.range(0);
    const std::string &file = bracketFiles[idx];
    size_t sz = sizes[idx];

    for (auto _ : state) {
        bool balanced = bb.naiveCheck(file);
        benchmark::DoNotOptimize(balanced);
    }
    state.SetBytesProcessed(int64_t(state.iterations()) * sz);
}

// Google Benchmark для bracket SIMD
static void BM_SIMDBracket(benchmark::State& state) {
    size_t idx = state.range(0);
    const std::string &file = bracketFiles[idx];
    size_t sz = sizes[idx];

    for (auto _ : state) {
        bool balanced = bb.simdCheck(file);
        benchmark::DoNotOptimize(balanced);
    }
    state.SetBytesProcessed(int64_t(state.iterations()) * sz);
}

int main(int argc, char** argv) {
    prepareFiles();

    benchmark::Initialize(&argc, argv);

    for (size_t i = 0; i < sizes.size(); ++i) {
        benchmark::RegisterBenchmark("NaiveSubstring", BM_NaiveSubstring)->Arg(i);
        benchmark::RegisterBenchmark("SIMDSubstring", BM_SIMDSubstring)->Arg(i);
        benchmark::RegisterBenchmark("NaiveBracket", BM_NaiveBracket)->Arg(i);
        benchmark::RegisterBenchmark("SIMDBracket", BM_SIMDBracket)->Arg(i);
    }

    benchmark::RunSpecifiedBenchmarks();
}
