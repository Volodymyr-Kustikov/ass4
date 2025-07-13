#ifndef CRYPTOGRAPHYSERVICE_H
#define CRYPTOGRAPHYSERVICE_H

#include "CipherManager.h"
#include "InputHandler.h"
#include "FileManager.h"
#include "../main.h"

class CryptographyService {
private:
    CipherManager cipherManager;
    InputHandler inputHandler;
    FileManager fileManager;
public:
    CryptographyService();
    void encryptCurrentText(TextBuffer* buffer);
    void decryptCurrentText(TextBuffer* buffer);
    void encryptTextFile();
    void decryptTextFile();
    void saveEncryptedText(TextBuffer* buffer);
    void loadEncryptedText(TextBuffer* buffer);

private:
    void saveBufferState(TextBuffer* buffer);
    bool updateBuffer(TextBuffer* buffer, const std::vector<char>& data);
};

#endif