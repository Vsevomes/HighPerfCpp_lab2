#include "substring_search.h"
#include <immintrin.h>

void SubstringSearch::generateData(const std::string &filename, size_t dataSize, size_t patternSize) {
    if (patternSize == 0 || dataSize == 0 || patternSize > dataSize) {
        throw std::runtime_error("Invalid sizes");
    }

    std::mt19937_64 rng((unsigned)std::chrono::high_resolution_clock::now().time_since_epoch().count());
    std::uniform_int_distribution<int> dist('a', 'z');

    std::vector<char> buffer(dataSize);
    for (size_t i = 0; i < dataSize; ++i) buffer[i] = static_cast<char>(dist(rng));

    // generate a pattern and insert it at a random position so searches can find it
    std::string pattern;
    pattern.resize(patternSize);
    for (size_t i = 0; i < patternSize; ++i) pattern[i] = static_cast<char>(dist(rng));

    std::uniform_int_distribution<size_t> posdist(0, dataSize - patternSize);
    size_t insert_pos = posdist(rng);
    std::memcpy(&buffer[insert_pos], pattern.data(), patternSize);

    // write data file
    std::ofstream ofs(filename, std::ios::binary);
    if (!ofs) throw std::runtime_error("Cannot open file for writing: " + filename);
    ofs.write(buffer.data(), static_cast<std::streamsize>(buffer.size()));
    ofs.close();

    // write pattern to adjacent file so the caller can read it
    std::string pattern_file = filename + ".pattern.txt";
    std::ofstream pf(pattern_file, std::ios::binary);
    if (!pf) throw std::runtime_error("Cannot open pattern file for writing: " + pattern_file);
    pf.write(pattern.data(), static_cast<std::streamsize>(pattern.size()));
    pf.close();
}

static std::vector<char> loadFile(const std::string &filename) {
    std::ifstream ifs(filename, std::ios::binary | std::ios::ate);
    if (!ifs) throw std::runtime_error("Cannot open file: " + filename);
    std::streamsize size = ifs.tellg();
    ifs.seekg(0, std::ios::beg);
    std::vector<char> buf(static_cast<size_t>(size));
    if (!ifs.read(buf.data(), size)) throw std::runtime_error("Failed to read file: " + filename);
    return buf;
}

bool SubstringSearch::naiveSearch(const std::string &filename, std::string_view pattern) const {
    if (pattern.empty()) return true;

    auto buf = loadFile(filename);
    size_t n = buf.size();
    size_t m = pattern.size();
    if (m > n) return false;

    const char *data = buf.data();
    const char *pat = pattern.data();

    for (size_t i = 0; i + m <= n; ++i) {
        // simple memcmp for each position
        if (std::memcmp(data + i, pat, m) == 0) return true;
    }
    return false;
}

bool SubstringSearch::simdSearch(const std::string &filename, std::string_view pattern) const {
    if (pattern.empty()) return true;

    auto buf = loadFile(filename);
    size_t n = buf.size();
    size_t m = pattern.size();
    if (m > n) return false;

    const unsigned char *data = reinterpret_cast<const unsigned char*>(buf.data());
    const unsigned char first = static_cast<unsigned char>(pattern[0]);

    size_t i = 0;
    const size_t step = 32;
    __m256i first_vec = _mm256_set1_epi8(static_cast<char>(first));

    for (; i + step <= n; i += step) {
        __m256i block = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(data + i));
        __m256i cmp = _mm256_cmpeq_epi8(block, first_vec);
        uint32_t mask = static_cast<uint32_t>(_mm256_movemask_epi8(cmp));
        while (mask) {
            unsigned tz = __builtin_ctz(mask);
            size_t pos = i + tz;
            if (pos + m <= n) {
                if (std::memcmp(data + pos, pattern.data(), m) == 0) return true;
            }
            mask &= mask - 1; 
        }
    }

    // tail: byte-by-byte
    for (; i + m <= n; ++i) {
        if (data[i] == first) {
            if (std::memcmp(data + i, pattern.data(), m) == 0) return true;
        }
    }

    return false;
}

