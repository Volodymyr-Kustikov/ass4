#include "CryptographyService.h"
#include <iostream>
#include <cstring>

CryptographyService::CryptographyService() {
}

void CryptographyService::encryptCurrentText(TextBuffer* buffer) {
    if (!buffer || !buffer->content || buffer->used == 0) {
        std::cout << "Buffer is empty. Nothing to encrypt." << std::endl;
        return;
    }

    int key = inputHandler.getIntegerInput("Enter encryption key (integer): ");

    saveBufferState(buffer);

    std::vector<char> data(buffer->content, buffer->content + buffer->used);
    std::vector<char> encryptedData = cipherManager.encryptText(data, key);

    if (encryptedData.empty()) {
        return;
    }
    if (updateBuffer(buffer, encryptedData)) {
        std::cout << "Text encrypted successfully with key " << key << "." << std::endl;
        saveBufferState(buffer);
    } else {
        std::cout << "Failed to update buffer with encrypted data." << std::endl;
    }
}

void CryptographyService::decryptCurrentText(TextBuffer* buffer) {
    if (!buffer || !buffer->content || buffer->used == 0) {
        std::cout << "Buffer is empty. Nothing to decrypt." << std::endl;
        return;
    }

    int key = inputHandler.getIntegerInput("Enter decryption key (integer): ");

    saveBufferState(buffer);
    std::vector<char> data(buffer->content, buffer->content + buffer->used);
    std::vector<char> decryptedData = cipherManager.decryptText(data, key);

    if (decryptedData.empty()) {
        return;
    }

    if (updateBuffer(buffer, decryptedData)) {
        std::cout << "Text decrypted successfully with key " << key << "." << std::endl;
        saveBufferState(buffer);
    } else {
        std::cout << "Failed to update buffer with decrypted data." << std::endl;
    }
}

void CryptographyService::encryptTextFile() {
    std::string inputPath = inputHandler.getStringInput("Enter input file path: ");
    std::string outputPath = inputHandler.getStringInput("Enter output file path: ");
    int key = inputHandler.getIntegerInput("Enter encryption key (integer): ");

    if (cipherManager.encryptFile(inputPath, outputPath, key)) {
        std::cout << "File encryption completed successfully." << std::endl;
    } else {
        std::cout << "File encryption failed." << std::endl;
    }
}

void CryptographyService::decryptTextFile() {
    std::string inputPath = inputHandler.getStringInput("Enter encrypted file path: ");
    std::string outputPath = inputHandler.getStringInput("Enter output file path: ");
    int key = inputHandler.getIntegerInput("Enter decryption key (integer): ");

    if (cipherManager.decryptFile(inputPath, outputPath, key)) {
        std::cout << "File decryption completed successfully." << std::endl;
    } else {
        std::cout << "File decryption failed." << std::endl;
    }
}

void CryptographyService::saveEncryptedText(TextBuffer* buffer) {
    if (!buffer || !buffer->content || buffer->used == 0) {
        std::cout << "Buffer is empty. Nothing to save." << std::endl;
        return;
    }

    std::string filename = inputHandler.getStringInput("Enter filename to save encrypted text: ");
    int key = inputHandler.getIntegerInput("Enter encryption key (integer): ");
    std::vector<char> data(buffer->content, buffer->content + buffer->used);
    std::vector<char> encryptedData = cipherManager.encryptText(data, key);

    if (encryptedData.empty()) {
        return;
    }

    if (fileManager.writeToFile(filename, encryptedData)) {
        std::cout << "Encrypted text saved successfully to: " << filename << std::endl;
    }
}

void CryptographyService::loadEncryptedText(TextBuffer* buffer) {
    std::string filename = inputHandler.getStringInput("Enter filename to load encrypted text: ");
    int key = inputHandler.getIntegerInput("Enter decryption key (integer): ");

    saveBufferState(buffer);

    std::vector<char> encryptedData = fileManager.readFromFile(filename);

    if (encryptedData.empty()) {
        std::cout << "File is empty or could not be read." << std::endl;
        return;
    }

    std::vector<char> decryptedData = cipherManager.decryptText(encryptedData, key);

    if (decryptedData.empty()) {
        return;
    }

    if (updateBuffer(buffer, decryptedData)) {
        std::cout << "Encrypted text loaded and decrypted successfully from: " << filename << std::endl;
        saveBufferState(buffer);
    } else {
        std::cout << "Memory allocation failed." << std::endl;
    }
}

void CryptographyService::saveBufferState(TextBuffer* buffer) {
    saveState(buffer);
}

bool CryptographyService::updateBuffer(TextBuffer* buffer, const std::vector<char>& data) {
    if (!buffer || data.empty()) {
        return false;
    }
    if (buffer->content) {
        free(buffer->content);
    }
    buffer->content = (char*)malloc(data.size() + 1);
    if (!buffer->content) {
        buffer->used = 0;
        buffer->size = 0;
        return false;
    }
    memcpy(buffer->content, data.data(), data.size());
    buffer->content[data.size()] = '\0';
    buffer->used = data.size();
    buffer->size = data.size() + 1;

    return true;
}
static CryptographyService cryptoService;
extern "C" void encryptCurrentText(TextBuffer* buffer) {
    cryptoService.encryptCurrentText(buffer);
}

extern "C" void decryptCurrentText(TextBuffer* buffer) {
    cryptoService.decryptCurrentText(buffer);
}

extern "C" void encryptTextFile() {
    cryptoService.encryptTextFile();
}

extern "C" void decryptTextFile() {
    cryptoService.decryptTextFile();
}

extern "C" void saveEncryptedText(TextBuffer* buffer) {
    cryptoService.saveEncryptedText(buffer);
}

extern "C" void loadEncryptedText(TextBuffer* buffer) {
    cryptoService.loadEncryptedText(buffer);
}