#ifndef RSA_H
#define RSA_H

#include <cstdint>
#include <string>
#include <vector>
#include <fstream>

inline uint64_t powmod(uint64_t base, uint64_t exp, uint64_t mod) {
    uint64_t result = 1;
    base %= mod;
    while (exp > 0) {
        if (exp & 1) result = (result * base) % mod;
        base = (base * base) % mod;
        exp >>= 1;
    }
    return result;
}

inline std::vector<uint64_t> encryptString(const std::string& text, uint64_t e, uint64_t n) {
    std::vector<uint64_t> cipher;
    for (unsigned char ch : text) cipher.push_back(powmod(ch, e, n));
    return cipher;
}

inline std::string decryptString(const std::vector<uint64_t>& cipher, uint64_t d, uint64_t n) {
    std::string text;
    for (uint64_t c : cipher) {
        uint64_t m = powmod(c, d, n);
        text.push_back(static_cast<char>(m & 0xFF));
    }
    return text;
}

inline bool isAsciiSpace(char ch) {
    return ch == ' ' || ch == '\t' || ch == '\n' || ch == '\r'
        || ch == '\v' || ch == '\f';
}

inline bool isEmptyFile(const std::string& filename) {
    std::ifstream file(filename, std::ios::binary);
    if (!file) return true;
    char ch;
    while (file.get(ch)) {
        if (!isAsciiSpace(ch)) return false;
    }
    return true;
}

inline bool writeCiphertext(const std::string& filename, const std::vector<uint64_t>& cipher) {
    std::ofstream file(filename, std::ios::binary);
    if (!file) return false;
    for (size_t i = 0; i < cipher.size(); ++i) {
        if (i > 0) file << ' ';
        file << cipher[i];
    }
    file << '\n';
    return true;
}

inline std::vector<uint64_t> readCiphertext(const std::string& filename) {
    std::ifstream file(filename, std::ios::binary);
    if (!file) return {};
    std::vector<uint64_t> cipher;
    uint64_t val;
    while (file >> val) cipher.push_back(val);
    if (file.fail() && !file.eof()) return {};  
    return cipher;
}

inline void clearFile(const std::string& filename) {
    std::ofstream file(filename, std::ios::binary | std::ios::trunc);
}

struct KeyPair {
    uint64_t n = 0;
    uint64_t e = 0;
};

inline KeyPair loadKeys(const std::string& filename = "key.txt") {
    std::ifstream f(filename);
    KeyPair k;
    if (!f) return k;
    f >> k.n >> k.e;
    return k;
}

#endif