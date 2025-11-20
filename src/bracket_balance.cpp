#include "bracket_balance.h"
#include <immintrin.h>

static bool isOpen(char c) {
    return c == '(' || c == '[' || c == '{';
}
static bool isClose(char c) {
    return c == ')' || c == ']' || c == '}';
}
static bool matches(char o, char c) {
    return (o == '(' && c == ')') || (o == '[' && c == ']') || (o == '{' && c == '}');
}
static std::vector<char> loadFile(const std::string &filename) {
    std::ifstream ifs(filename, std::ios::binary | std::ios::ate);
    if (!ifs) throw std::runtime_error("Cannot open file: " + filename);
    std::streamsize size = ifs.tellg();
    ifs.seekg(0, std::ios::beg);
    std::vector<char> buf(static_cast<size_t>(size));
    if (!ifs.read(buf.data(), size)) throw std::runtime_error("Failed to read file");
    return buf;
}

void BracketBalance::generateData(const std::string &filename, size_t dataSize) {
    std::mt19937_64 rng((unsigned)std::chrono::high_resolution_clock::now().time_since_epoch().count());
    std::uniform_int_distribution<int> dist(0, 5);
    static const char brackets[6] = {'(', ')', '{', '}', '[', ']'};

    std::vector<char> buffer(dataSize);
    for (size_t i = 0; i < dataSize; ++i) buffer[i] = brackets[dist(rng)];

    std::ofstream ofs(filename, std::ios::binary);
    if (!ofs) throw std::runtime_error("Cannot open file for writing: " + filename);
    ofs.write(buffer.data(), buffer.size());
}

bool BracketBalance::naiveCheck(const std::string &filename) const {
    auto buf = loadFile(filename);
    std::vector<char> stack;
    stack.reserve(buf.size());

    for (char c : buf) {
        if (isOpen(c)) {
            stack.push_back(c);
        } else if (isClose(c)) {
            if (stack.empty()) return false;
            if (!matches(stack.back(), c)) return false;
            stack.pop_back();
        }
    }
    return stack.empty();
}

bool BracketBalance::simdCheck(const std::string &filename) const {
    auto buf = loadFile(filename);
    const size_t n = buf.size();
    const char *data = buf.data();
    
    // SIMD может только ускорить поиск скобок, но проверка стека должна быть последовательной
    // Поэтому используем гибридный подход: находим скобки с SIMD, проверяем стеком
    
    std::vector<char> stack;
    stack.reserve(n / 2);
    
    const size_t step = 32;
    size_t i = 0;
    
    __m256i open_round = _mm256_set1_epi8('(');
    __m256i close_round = _mm256_set1_epi8(')');
    __m256i open_square = _mm256_set1_epi8('[');
    __m256i close_square = _mm256_set1_epi8(']');
    __m256i open_curly = _mm256_set1_epi8('{');
    __m256i close_curly = _mm256_set1_epi8('}');
    
    for (; i + step <= n; i += step) {
        __m256i block = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(data + i));
        
        int open_r_mask = _mm256_movemask_epi8(_mm256_cmpeq_epi8(block, open_round));
        int close_r_mask = _mm256_movemask_epi8(_mm256_cmpeq_epi8(block, close_round));
        int open_s_mask = _mm256_movemask_epi8(_mm256_cmpeq_epi8(block, open_square));
        int close_s_mask = _mm256_movemask_epi8(_mm256_cmpeq_epi8(block, close_square));
        int open_c_mask = _mm256_movemask_epi8(_mm256_cmpeq_epi8(block, open_curly));
        int close_c_mask = _mm256_movemask_epi8(_mm256_cmpeq_epi8(block, close_curly));
        
        for (int j = 0; j < 32; j++) {
            size_t pos = i + j;
            if (pos >= n) break;
            
            char c = data[pos];
            if (open_r_mask & (1 << j)) {
                stack.push_back('(');
            } else if (close_r_mask & (1 << j)) {
                if (stack.empty() || stack.back() != '(') return false;
                stack.pop_back();
            } else if (open_s_mask & (1 << j)) {
                stack.push_back('[');
            } else if (close_s_mask & (1 << j)) {
                if (stack.empty() || stack.back() != '[') return false;
                stack.pop_back();
            } else if (open_c_mask & (1 << j)) {
                stack.push_back('{');
            } else if (close_c_mask & (1 << j)) {
                if (stack.empty() || stack.back() != '{') return false;
                stack.pop_back();
            }
        }
    }
    
    for (; i < n; ++i) {
        char c = data[i];
        if (isOpen(c)) {
            stack.push_back(c);
        } else if (isClose(c)) {
            if (stack.empty() || !matches(stack.back(), c)) return false;
            stack.pop_back();
        }
    }
    
    return stack.empty();
}