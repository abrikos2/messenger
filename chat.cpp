#include "rsa.h"
#include <iostream>
#include <string>
#include <thread>
#include <chrono>
#include <atomic>
#include <limits>

#ifdef _WIN32
  #include <windows.h>
#endif

int main(int argc, char* argv[]) {
#ifdef _WIN32
    SetConsoleCP(CP_UTF8);
    SetConsoleOutputCP(CP_UTF8);
#endif

    if (argc < 2) {
        std::cout << "Usage: " << argv[0] << " 1|2\n";
        return 1;
    }
    int role = std::atoi(argv[1]);
    if (role != 1 && role != 2) {
        std::cout << "Role must be 1 or 2\n";
        return 1;
    }
    int other = 3 - role;

    std::string myKeysFile    = "keys"    + std::to_string(role)  + ".txt";
    std::string otherKeysFile = "keys"    + std::to_string(other) + ".txt";
    std::string outgoing      = "channel" + std::to_string(role)  + std::to_string(other) + ".txt";
    std::string incoming      = "channel" + std::to_string(other) + std::to_string(role)  + ".txt";

    KeyPair mine   = loadKeys(myKeysFile);
    KeyPair theirs = loadKeys(otherKeysFile);
    if (mine.n == 0 || theirs.n == 0) {
        std::cout << "Нет файлов ключей. Сначала запустите keygen.\n";
        return 1;
    }

    std::cout << "Роль " << role << '\n';
    std::cout << "user1  n = " << mine.n   << ", e = " << mine.e   << '\n';
    std::cout << "user2 n = " << theirs.n << ", e = " << theirs.e << '\n';

    std::cout << "Введите мой приватный ключ (d): ";
    uint64_t d;
    while (!(std::cin >> d)) {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "Введите целое число (d): ";
    }
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    std::atomic<bool> running{true};

    std::thread recv([&]() {
        while (running) {
            if (!isEmptyFile(incoming)) {
                auto cipher = readCiphertext(incoming);
                if (!cipher.empty()) {
                    std::string msg = decryptString(cipher, d, mine.n);
                    std::cout << "\n[Входящее]: " << msg << "\n> " << std::flush;
                }
                clearFile(incoming);
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(300));
        }
    });

    std::cout << "> " << std::flush;
    std::string line;
    while (std::getline(std::cin, line)) {
        if (line.empty()) { std::cout << "> " << std::flush; continue; }
        if (line == "exit" || line == "quit") break;

        while (!isEmptyFile(outgoing)) {
            std::this_thread::sleep_for(std::chrono::milliseconds(200));
        }

        auto cipher = encryptString(line, theirs.e, theirs.n);
        writeCiphertext(outgoing, cipher);
        std::cout << "> " << std::flush;
    }

    running = false;
    recv.join();
    return 0;
}