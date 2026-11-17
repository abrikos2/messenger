#include <cstdint>
#include <iostream>
#include <fstream>
#include <random>

#ifdef _WIN32
  #include <windows.h>
#endif

static bool isPrime(uint64_t x) {
    if (x < 2) return false;
    if (x % 2 == 0) return x == 2;
    for (uint64_t i = 3; i * i <= x; i += 2)
        if (x % i == 0) return false;
    return true;
}

static uint64_t gcd64(uint64_t a, uint64_t b) {
    while (b) { uint64_t t = a % b; a = b; b = t; }
    return a;
}

static uint64_t modinv(uint64_t a, uint64_t m) {
    a %= m;
    for (uint64_t d = 1; d < m; ++d)
        if ((a * d) % m == 1) return d;
    return 0;
}

struct FullKey {
    uint64_t p, q, n, phi, e, d;
};

static FullKey generate() {
    FullKey k;
    std::random_device rd;
    std::mt19937_64 gen(rd());
    std::uniform_int_distribution<uint64_t> dist(100, 1000);
    auto rp = [&]() {
        while (true) {
            uint64_t c = dist(gen);
            if (isPrime(c)) return c;
        }
    };
    while (true) {
        k.p = rp();
        k.q = rp();
        if (k.p == k.q) continue;
        k.n = k.p * k.q;
        if (k.n <= 255) continue;
        k.phi = (k.p - 1) * (k.q - 1);
        break;
    }
    k.e = 0;
    for (uint64_t c : {3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37, 41, 43}) {
        if (gcd64(c, k.phi) == 1) { k.e = c; break; }
    }
    if (k.e == 0) {
        for (uint64_t c = 3; c < 10000; ++c)
            if (gcd64(c, k.phi) == 1) { k.e = c; break; }
    }
    k.d = modinv(k.e, k.phi);
    return k;
}

int main() {
#ifdef _WIN32
    SetConsoleCP(CP_UTF8);
    SetConsoleOutputCP(CP_UTF8);
#endif

    FullKey a = generate();
    FullKey b = generate();

    std::cout << "User 1\n";
    std::cout << "p= "   << a.p   << '\n';
    std::cout << "q= "   << a.q   << '\n';
    std::cout << "n= "   << a.n   << '\n';
    std::cout << "phi= " << a.phi << '\n';
    std::cout << "e= "   << a.e   << '\n';
    std::cout << "d= "   << a.d   << "\n\n";

    std::cout << "User 2\n";
    std::cout << "p= "   << b.p   << '\n';
    std::cout << "q= "   << b.q   << '\n';
    std::cout << "n= "   << b.n   << '\n';
    std::cout << "phi= " << b.phi << '\n';
    std::cout << "e= "   << b.e   << '\n';
    std::cout << "d= "   << b.d   << '\n';

    std::ofstream f1("keys1.txt");
    std::ofstream f2("keys2.txt");
    if (!f1 || !f2) return 1;
    f1 << a.n << ' ' << a.e << '\n';
    f2 << b.n << ' ' << b.e << '\n';
    return 0;
}