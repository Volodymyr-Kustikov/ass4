#include <iostream>
#include <cstring>
#include <cstdlib>
#include "../main.h"


#define MIN(a,b) ((a)<(b)?(a):(b))
#define MAX(a,b) ((a)>(b)?(a):(b))

UndoRedoHistory history;
char clipboard[1024] = {0};

extern "C" void initHistory(void) {
    for(int i = 0; i < 10; i++) {
        history.states[i] = NULL;
        history.stateSizes[i] = 0;
    }
    history.currentIndex = -1;
    history.totalStates = 0;
}

extern "C" void saveState(TextBuffer* buffer) {
    if (!buffer || !buffer->content) return;

    history.currentIndex = (history.currentIndex + 1) % 10;

    if (history.states[history.currentIndex] != NULL) {
        free(history.states[history.currentIndex]);
    }

    history.stateSizes[history.currentIndex] = buffer->used + 1;
    history.states[history.currentIndex] = (char*)malloc(history.stateSizes[history.currentIndex]);

    if (history.states[history.currentIndex] != NULL) {
        strncpy(history.states[history.currentIndex], buffer->content, buffer->used);
        history.states[history.currentIndex][buffer->used] = '\0';

        if (history.totalStates < 10) {
            history.totalStates++;
        }
    }
}

extern "C" void undoCommand(TextBuffer* buffer) {
    if (history.totalStates <= 1) {
        std::cout << "Nothing to undo." << std::endl;
        return;
    }

    int prevIndex = (history.currentIndex - 1 + 10) % 10;

    if (history.states[prevIndex] != NULL) {
        strncpy(buffer->content, history.states[prevIndex], buffer->size - 1);
        buffer->content[buffer->size - 1] = '\0';
        buffer->used = strlen(buffer->content);

        history.currentIndex = prevIndex;
        history.totalStates--;

        std::cout << "Undo completed." << std::endl;
    } else {
        std::cout << "Cannot undo - previous state not available." << std::endl;
    }
}

extern "C" void redoCommand(TextBuffer* buffer) {
    int nextIndex = (history.currentIndex + 1) % 10;

    if (history.states[nextIndex] != NULL && history.totalStates < 10) {
        strncpy(buffer->content, history.states[nextIndex], buffer->size - 1);
        buffer->content[buffer->size - 1] = '\0';
        buffer->used = strlen(buffer->content);

        history.currentIndex = nextIndex;
        history.totalStates++;

        std::cout << "Redo completed." << std::endl;
    } else {
        std::cout << "Nothing to redo." << std::endl;
    }
}

extern "C" void freeHistory(void) {
    for(int i = 0; i < 10; i++) {
        if(history.states[i] != NULL) {
            free(history.states[i]);
            history.states[i] = NULL;
        }
        history.stateSizes[i] = 0;
    }
    history.currentIndex = -1;
    history.totalStates = 0;
}

int findPosition(TextBuffer* buffer, int line, int index) {
    if (buffer->used == 0) return -1;

    int currentLine = 0;
    int pos = 0;

    while (pos < buffer->used && currentLine < line) {
        if (buffer->content[pos] == '\n') {
            currentLine++;
        }
        pos++;
    }

    if (currentLine < line) return -1;

    int lineStart = pos;
    int lineLength = 0;
    while ((lineStart + lineLength) < buffer->used && buffer->content[lineStart + lineLength] != '\n') {
        lineLength++;
    }

    if (index > lineLength) return -1;

    return pos + index;
}


extern "C" void deleteText(TextBuffer* buffer) {
    saveState(buffer);

    int line, index, numberOfChar;
    std::cout << "Choose line, index and number of symbols: ";
    if (!(std::cin >> line >> index >> numberOfChar)) {
        std::cout << "Invalid input format." << std::endl;
        std::cin.clear(); std::cin.ignore(1000, '\n'); return;
    }

    if (line < 0 || index < 0 || numberOfChar < 0) {
        std::cout << "Error: All values must be non-negative." << std::endl;
        return;
    }

    int startPos = findPosition(buffer, line, index);
    if (startPos == -1 || startPos >= buffer->used) {
        std::cout << "Error: Invalid line or index." << std::endl;
        return;
    }

    int actualDelete = MIN(numberOfChar, buffer->used - startPos);
    memmove(buffer->content + startPos, buffer->content + startPos + actualDelete, buffer->used - startPos - actualDelete + 1);
    buffer->used -= actualDelete;

    std::cout << "Deleted " << actualDelete << " character(s)." << std::endl;
    saveState(buffer);
}

extern "C" void copyText(TextBuffer* buffer) {
    int line, index, numberOfChar;

    std::cout << "Choose line, index and number of symbols: ";
    if (!(std::cin >> line >> index >> numberOfChar)) {
        std::cout << "Invalid input format." << std::endl;
        std::cin.clear();
        std::cin.ignore(1000, '\n');
        return;
    }

    if (line < 0 || index < 0 || numberOfChar < 0) {
        std::cout << "Error: All values must be non-negative." << std::endl;
        return;
    }

    int startPos = findPosition(buffer, line, index);
    if (startPos == -1) {
        std::cout << "Error: Invalid line or index." << std::endl;
        return;
    }

    int maxCopy = buffer->used - startPos;
    int actualCopy = (numberOfChar > maxCopy) ? maxCopy : numberOfChar;

    if (actualCopy > 0 && actualCopy < 1024) {
        strncpy(clipboard, buffer->content + startPos, actualCopy);
        clipboard[actualCopy] = '\0';
        std::cout << "Copied " << actualCopy << " character(s) to clipboard." << std::endl;
    } else {
        std::cout << "Nothing to copy or text too long for clipboard." << std::endl;
    }
}

extern "C" void cutText(TextBuffer* buffer) {
    saveState(buffer);

    int line, index, numberOfChar;

    std::cout << "Choose line, index and number of symbols: ";
    if (!(std::cin >> line >> index >> numberOfChar)) {
        std::cout << "Invalid input format." << std::endl;
        std::cin.clear();
        std::cin.ignore(1000, '\n');
        return;
    }

    if (line < 0 || index < 0 || numberOfChar < 0) {
        std::cout << "Error: All values must be non-negative." << std::endl;
        return;
    }

    int startPos = findPosition(buffer, line, index);
    if (startPos == -1) {
        std::cout << "Error: Invalid line or index." << std::endl;
        return;
    }

    int maxCut = buffer->used - startPos;
    int actualCut = (numberOfChar > maxCut) ? maxCut : numberOfChar;

    if (actualCut > 0 && actualCut < 1024) {
        strncpy(clipboard, buffer->content + startPos, actualCut);
        clipboard[actualCut] = '\0';

        memmove(buffer->content + startPos,
                buffer->content + startPos + actualCut,
                buffer->used - startPos - actualCut + 1);

        buffer->used -= actualCut;
        std::cout << "Cut " << actualCut << " character(s) to clipboard." << std::endl;

        saveState(buffer);
    } else {
        std::cout << "Nothing to cut or text too long for clipboard." << std::endl;
    }
}

extern "C" void pasteText(TextBuffer* buffer) {
    if (strlen(clipboard) == 0) {
        std::cout << "Clipboard is empty." << std::endl;
        return;
    }

    saveState(buffer);

    int line, index;

    std::cout << "Choose line and index: ";
    if (!(std::cin >> line >> index)) {
        std::cout << "Invalid input format." << std::endl;
        std::cin.clear();
        std::cin.ignore(1000, '\n');
        return;
    }

    if (line < 0 || index < 0) {
        std::cout << "Error: Line and index must be non-negative." << std::endl;
        return;
    }

    int pastePos = findPosition(buffer, line, index);
    if (pastePos == -1) {
        std::cout << "Error: Invalid line or index." << std::endl;
        return;
    }

    size_t clipboardLen = strlen(clipboard);

    if (buffer->used + clipboardLen + 1 > buffer->size) {
        size_t newSize = buffer->size + clipboardLen + 1024;
        char* newBuffer = (char*)realloc(buffer->content, newSize);
        if (newBuffer == NULL) {
            std::cout << "Error: Memory allocation failed." << std::endl;
            return;
        }
        buffer->content = newBuffer;
        buffer->size = newSize;
    }

    memmove(buffer->content + pastePos + clipboardLen,
            buffer->content + pastePos,
            buffer->used - pastePos + 1);

    memcpy(buffer->content + pastePos, clipboard, clipboardLen);
    buffer->used += clipboardLen;

    buffer->content[buffer->used] = '\0';

    std::cout << "Pasted " << clipboardLen << " character(s) from clipboard." << std::endl;

    saveState(buffer);
}

extern "C" void insertWithReplacement(TextBuffer* buffer) {
    saveState(buffer);

    int line, index;
    char input[1024];
    std::cout << "Choose line and index: ";
    if (!(std::cin >> line >> index)) {
        std::cout << "Invalid input format." << std::endl;
        std::cin.clear(); std::cin.ignore(1000, '\n'); return;
    }
    std::cin.ignore();

    std::cout << "Write text: ";
    if (!std::cin.getline(input, sizeof(input))) {
        std::cout << "Error reading input." << std::endl;
        return;
    }

    int insertPos = findPosition(buffer, line, index);
    if (insertPos == -1 || insertPos >= buffer->used) {
        std::cout << "Error: Invalid line or index." << std::endl;
        return;
    }

    size_t inputLen = strlen(input);
    int maxReplace = std::min((int)(buffer->size - insertPos - 1), (int)inputLen);
    memcpy(buffer->content + insertPos, input, maxReplace);
    buffer->used = MAX(buffer->used, insertPos + maxReplace);
    buffer->content[buffer->used] = '\0';

    std::cout << "Text inserted with replacement." << std::endl;
    saveState(buffer);
}