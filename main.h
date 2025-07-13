#ifndef MAIN_H
#define MAIN_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    char* content;
    size_t size;
    size_t used;
} TextBuffer;

typedef struct {
    char* states[10];
    size_t stateSizes[10];
    int currentIndex;
    int totalStates;
} UndoRedoHistory;

typedef enum {
    DATA_TYPE_TEXT = 0,
    DATA_TYPE_CONTACT = 1,
    DATA_TYPE_CHECKLIST = 2
} DataType;

typedef struct {
    char name[100];
    char surname[100];
    char email[150];
} ContactInfo;

typedef struct {
    char info[200];
    int checked;
} ChecklistItem;

typedef struct {
    DataType type;
    union {
        char* text;
        ContactInfo contact;
        ChecklistItem checklist;
    } data;
} LineData;

typedef struct {
    LineData* lines;
    size_t lineCount;
    size_t capacity;
} Document;

extern Document* g_document;

void displayMenu(void);
void processUserOption(int userOption, TextBuffer* buffer);
void initializeBuffer(TextBuffer* buffer);
void resizeBufferIfNeeded(TextBuffer* buffer, size_t additionalSpace);
void freeBuffer(TextBuffer* buffer);
void clearConsole(void);
void clearInputBuffer(void);

void appendText(TextBuffer* buffer);
void addNewLine(TextBuffer* buffer);
void saveToFile(TextBuffer* buffer);
void loadFromFile(TextBuffer* buffer);
void printCurrentText(TextBuffer* buffer);
void insertTextAtPosition(TextBuffer* buffer);
void searchText(TextBuffer* buffer);
void deleteText(TextBuffer* buffer);

void initHistory(void);
void saveState(TextBuffer* buffer);
void undoCommand(TextBuffer* buffer);
void redoCommand(TextBuffer* buffer);
void pasteText(TextBuffer* buffer);
void copyText(TextBuffer* buffer);
void cutText(TextBuffer* buffer);
void insertWithReplacement(TextBuffer* buffer);
void freeHistory(void);

void encryptCurrentText(TextBuffer* buffer);
void decryptCurrentText(TextBuffer* buffer);
void encryptTextFile(void);
void decryptTextFile(void);
void saveEncryptedText(TextBuffer* buffer);
void loadEncryptedText(TextBuffer* buffer);

void initDataHandler(Document* doc);

void addContactLine();
void editContactLine();

#ifdef __cplusplus
}
#endif

#endif