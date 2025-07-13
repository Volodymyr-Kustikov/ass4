#include "CaesarCipher.h"
#include <iostream>
#include <fstream>
#include <cstring>

CaesarCipher::CaesarCipher() : libraryHandle(nullptr), encryptFunc(nullptr), decryptFunc(nullptr) {
    if (!loadLibrary()) {
        std::cerr << "Failed to load Caesar cipher library" << std::endl;
        return;
    }

    if (!loadFunctions()) {
        std::cerr << "Failed to load Caesar cipher functions" << std::endl;
        unloadLibrary();
        return;
    }
}

CaesarCipher::~CaesarCipher() {
    unloadLibrary();
}

bool CaesarCipher::loadLibrary() {
    libraryHandle = dlopen("./libcaesar.dylib", RTLD_LAZY);
    if (!libraryHandle) {
        libraryHandle = dlopen("libcaesar.dylib", RTLD_LAZY);
        if (!libraryHandle) {
            std::cerr << "Failed to load libcaesar.dylib: " << dlerror() << std::endl;
            return false;
        }
    }
    return true;
}

void CaesarCipher::unloadLibrary() {
    if (libraryHandle) {
        dlclose(libraryHandle);
        libraryHandle = nullptr;
        encryptFunc = nullptr;
        decryptFunc = nullptr;
    }
}

bool CaesarCipher::loadFunctions() {
    if (!libraryHandle) return false;

    encryptFunc = (EncryptFunction)dlsym(libraryHandle, "caesar_encrypt");
    decryptFunc = (DecryptFunction)dlsym(libraryHandle, "caesar_decrypt");

    if (!encryptFunc || !decryptFunc) {
        std::cerr << "Failed to load Caesar cipher functions from library" << std::endl;
        return false;
    }

    return true;
}

bool CaesarCipher::isReady() const {
    return (libraryHandle != nullptr && encryptFunc != nullptr && decryptFunc != nullptr);
}

std::vector<char> CaesarCipher::encrypt(const std::vector<char>& data, int key) {
    if (!isReady()) {
        std::cerr << "Caesar cipher not ready" << std::endl;
        return std::vector<char>();
    }

    std::vector<char> result(data.size());

    if (!data.empty()) {
        encryptFunc(data.data(), result.data(), key, static_cast<int>(data.size()));
    }

    return result;
}

std::vector<char> CaesarCipher::decrypt(const std::vector<char>& data, int key) {
    if (!isReady()) {
        std::cerr << "Caesar cipher not ready" << std::endl;
        return std::vector<char>();
    }

    std::vector<char> result(data.size());

    if (!data.empty()) {
        decryptFunc(data.data(), result.data(), key, static_cast<int>(data.size()));
    }

    return result;
}

std::string CaesarCipher::encrypt(const std::string& text, int key) {
    if (!isReady()) {
        std::cerr << "Caesar cipher not ready" << std::endl;
        return "";
    }

    std::vector<char> input(text.begin(), text.end());
    std::vector<char> encrypted = encrypt(input, key);

    return std::string(encrypted.begin(), encrypted.end());
}

std::string CaesarCipher::decrypt(const std::string& text, int key) {
    if (!isReady()) {
        std::cerr << "Caesar cipher not ready" << std::endl;
        return "";
    }

    std::vector<char> input(text.begin(), text.end());
    std::vector<char> decrypted = decrypt(input, key);

    return std::string(decrypted.begin(), decrypted.end());
}

bool CaesarCipher::encryptFile(const std::string& inputPath, const std::string& outputPath, int key) {
    if (!isReady()) {
        std::cerr << "Caesar cipher not ready" << std::endl;
        return false;
    }
    std::ifstream inputFile(inputPath, std::ios::binary);
    if (!inputFile.is_open()) {
        std::cerr << "Failed to open input file: " << inputPath << std::endl;
        return false;
    }
    std::vector<char> fileData((std::istreambuf_iterator<char>(inputFile)),
                               std::istreambuf_iterator<char>());
    inputFile.close();
    if (fileData.empty()) {
        std::cerr << "Input file is empty or could not be read" << std::endl;
        return false;
    }
    std::vector<char> encryptedData = encrypt(fileData, key);

    if (encryptedData.empty()) {
        std::cerr << "Encryption failed" << std::endl;
        return false;
    }

    std::ofstream outputFile(outputPath, std::ios::binary);
    if (!outputFile.is_open()) {
        std::cerr << "Failed to open output file: " << outputPath << std::endl;
        return false;
    }

    outputFile.write(encryptedData.data(), encryptedData.size());
    outputFile.close();

    std::cout << "File encrypted successfully: " << inputPath << " -> " << outputPath << std::endl;
    return true;
}

bool CaesarCipher::decryptFile(const std::string& inputPath, const std::string& outputPath, int key) {
    if (!isReady()) {
        std::cerr << "Caesar cipher not ready" << std::endl;
        return false;
    }

    std::ifstream inputFile(inputPath, std::ios::binary);
    if (!inputFile.is_open()) {
        std::cerr << "Failed to open encrypted file: " << inputPath << std::endl;
        return false;
    }

    std::vector<char> encryptedData((std::istreambuf_iterator<char>(inputFile)),
                                    std::istreambuf_iterator<char>());
    inputFile.close();

    if (encryptedData.empty()) {
        std::cerr << "Encrypted file is empty or could not be read" << std::endl;
        return false;
    }

    std::vector<char> decryptedData = decrypt(encryptedData, key);

    if (decryptedData.empty()) {
        std::cerr << "Decryption failed" << std::endl;
        return false;
    }

    std::ofstream outputFile(outputPath, std::ios::binary);
    if (!outputFile.is_open()) {
        std::cerr << "Failed to open output file: " << outputPath << std::endl;
        return false;
    }

    outputFile.write(decryptedData.data(), decryptedData.size());
    outputFile.close();

    std::cout << "File decrypted successfully: " << inputPath << " -> " << outputPath << std::endl;
    return true;
}