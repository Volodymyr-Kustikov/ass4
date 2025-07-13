#ifndef FILEMANAGER_H
#define FILEMANAGER_H

#include <string>
#include <vector>
#include <fstream>

class FileManager {
public:
    static bool writeToFile(const std::string& filename, const std::vector<char>& data);
    static std::vector<char> readFromFile(const std::string& filename);
    static bool fileExists(const std::string& filename);
    static bool isValidPath(const std::string& path);
};

#endif