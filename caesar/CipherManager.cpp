#include "CipherManager.h"
#include <iostream>

CipherManager::CipherManager() {
}

std::vector<char> CipherManager::encryptText(const std::vector<char>& data, int key) {
    if (!cipher.isReady()) {
        std::cout << "Caesar cipher is not ready. Please check if the DLL is properly loaded." << std::endl;
        return std::vector<char>();
    }

    if (!validateData(data)) {
        std::cout << "Invalid data for encryption." << std::endl;
        return std::vector<char>();
    }

    std::vector<char> result = cipher.encrypt(data, key);
    if (result.empty()) {
        std::cout << "Encryption failed." << std::endl;
    }

    return result;
}

std::vector<char> CipherManager::decryptText(const std::vector<char>& data, int key) {
    if (!cipher.isReady()) {
        std::cout << "Caesar cipher is not ready. Please check if the DLL is properly loaded." << std::endl;
        return std::vector<char>();
    }

    if (!validateData(data)) {
        std::cout << "Invalid data for decryption." << std::endl;
        return std::vector<char>();
    }

    std::vector<char> result = cipher.decrypt(data, key);
    if (result.empty()) {
        std::cout << "Decryption failed." << std::endl;
    }

    return result;
}

bool CipherManager::encryptFile(const std::string& inputPath, const std::string& outputPath, int key) {
    if (!cipher.isReady()) {
        std::cout << "Caesar cipher is not ready. Please check if the DLL is properly loaded." << std::endl;
        return false;
    }

    return cipher.encryptFile(inputPath, outputPath, key);
}

bool CipherManager::decryptFile(const std::string& inputPath, const std::string& outputPath, int key) {
    if (!cipher.isReady()) {
        std::cout << "Caesar cipher is not ready. Please check if the DLL is properly loaded." << std::endl;
        return false;
    }

    return cipher.decryptFile(inputPath, outputPath, key);
}

bool CipherManager::validateKey(int key) const {
    return true;
}

bool CipherManager::validateData(const std::vector<char>& data) const {
    return !data.empty();
}
