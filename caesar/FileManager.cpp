#include "FileManager.h"
#include <iostream>

bool FileManager::writeToFile(const std::string& filename, const std::vector<char>& data) {
    std::ofstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        std::cout << "Failed to open file for writing: " << filename << std::endl;
        return false;
    }
    file.write(data.data(), data.size());
    file.close();
    return true;
}

std::vector<char> FileManager::readFromFile(const std::string& filename) {
    std::ifstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        std::cout << "Failed to open file for reading: " << filename << std::endl;
        return std::vector<char>();
    }
    std::vector<char> data((std::istreambuf_iterator<char>(file)),std::istreambuf_iterator<char>());
    file.close();
    return data;
}

bool FileManager::fileExists(const std::string& filename) {
    std::ifstream file(filename);
    return file.good();
}

bool FileManager::isValidPath(const std::string& path) {
    return !path.empty();
}