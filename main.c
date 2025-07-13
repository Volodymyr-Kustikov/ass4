#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "main.h"

#define INITIAL_BUFFER_SIZE 1024
#define MAX_FILENAME_LENGTH 100
#define MAX_INPUT_LENGTH 1024
#define CONTACTS_FILE "contacts.txt"

Document* g_document = NULL;

void displayMenu(void);
void processUserOption(int userOption, TextBuffer* buffer);
void initializeBuffer(TextBuffer* buffer);
void resizeBufferIfNeeded(TextBuffer* buffer, size_t additionalSpace);
void freeBuffer(TextBuffer* buffer);
void appendText(TextBuffer* buffer);
void addNewLine(TextBuffer* buffer);
void saveToFile(TextBuffer* buffer);
void loadFromFile(TextBuffer* buffer);
void printCurrentText(TextBuffer* buffer);
void insertTextAtPosition(TextBuffer* buffer);
void searchText(TextBuffer* buffer);
void clearConsole(void);
void clearInputBuffer(void);
void deleteText(TextBuffer* buffer);

Document* createDocument(void);
void freeDocument(Document* doc);
void initDataHandler(Document* doc);
void addContactLine(void);
void editContactLine(void);
void saveContactsToFile(void);
void loadContactsFromFile(void);
void printAllContacts(void);
void ensureContactCapacity(size_t requiredCapacity);

void encryptCurrentText(TextBuffer* buffer);
void decryptCurrentText(TextBuffer* buffer);
void encryptTextFile(void);
void decryptTextFile(void);
void saveEncryptedText(TextBuffer* buffer);
void loadEncryptedText(TextBuffer* buffer);

void initHistory(void);
void saveState(TextBuffer* buffer);

Document* createDocument(void) {
    Document* doc = (Document*)malloc(sizeof(Document));
    if (!doc) {
        printf("Error: Failed to allocate memory for document\n");
        return NULL;
    }

    doc->lines = (LineData*)malloc(10 * sizeof(LineData));
    if (!doc->lines) {
        free(doc);
        printf("Error: Failed to allocate memory for lines\n");
        return NULL;
    }

    doc->lineCount = 0;
    doc->capacity = 10;
    return doc;
}

void initDataHandler(Document* doc) {
    g_document = doc;
    loadContactsFromFile();
}

void ensureContactCapacity(size_t requiredCapacity) {
    if (g_document->capacity < requiredCapacity) {
        size_t newCapacity = (requiredCapacity > g_document->capacity * 2) ?
                             requiredCapacity : g_document->capacity * 2;

        LineData* newLines = (LineData*)realloc(g_document->lines, newCapacity * sizeof(LineData));
        if (newLines) {
            g_document->lines = newLines;
            g_document->capacity = newCapacity;
        } else {
            printf("Error: Failed to allocate memory for contacts\n");
        }
    }
}

void saveContactsToFile(void) {
    FILE* file = fopen(CONTACTS_FILE, "w");
    if (!file) {
        printf("Error: Could not open contacts file for writing\n");
        return;
    }

    fprintf(file, "%zu\n", g_document->lineCount);

    for (size_t i = 0; i < g_document->lineCount; i++) {
        if (g_document->lines[i].type == DATA_TYPE_CONTACT) {
            fprintf(file, "%d|%s|%s|%s\n",
                    g_document->lines[i].type,
                    g_document->lines[i].data.contact.name,
                    g_document->lines[i].data.contact.surname,
                    g_document->lines[i].data.contact.email);
        }
    }

    fclose(file);
    printf("Contacts saved to %s\n", CONTACTS_FILE);
}

void loadContactsFromFile(void) {
    FILE* file = fopen(CONTACTS_FILE, "r");
    if (!file) {
        printf("No existing contacts file found. Starting fresh.\n");
        return;
    }

    size_t contactCount;
    if (fscanf(file, "%zu\n", &contactCount) != 1) {
        printf("Error reading contacts file format\n");
        fclose(file);
        return;
    }

    g_document->lineCount = 0;

    char line[500];
    while (fgets(line, sizeof(line), file) && g_document->lineCount < contactCount) {
        // Remove newline
        line[strcspn(line, "\n")] = 0;

        char* token = strtok(line, "|");
        if (!token) continue;

        int type = atoi(token);
        if (type != DATA_TYPE_CONTACT) continue;

        char* name = strtok(NULL, "|");
        char* surname = strtok(NULL, "|");
        char* email = strtok(NULL, "|");

        if (name && surname && email) {
            ensureContactCapacity(g_document->lineCount + 1);

            LineData* newLine = &g_document->lines[g_document->lineCount];
            newLine->type = DATA_TYPE_CONTACT;

            strncpy(newLine->data.contact.name, name, sizeof(newLine->data.contact.name) - 1);
            newLine->data.contact.name[sizeof(newLine->data.contact.name) - 1] = '\0';

            strncpy(newLine->data.contact.surname, surname, sizeof(newLine->data.contact.surname) - 1);
            newLine->data.contact.surname[sizeof(newLine->data.contact.surname) - 1] = '\0';

            strncpy(newLine->data.contact.email, email, sizeof(newLine->data.contact.email) - 1);
            newLine->data.contact.email[sizeof(newLine->data.contact.email) - 1] = '\0';

            g_document->lineCount++;
        }
    }

    fclose(file);
    printf("Loaded %zu contacts from %s\n", g_document->lineCount, CONTACTS_FILE);
}

void printAllContacts(void) {
    if (!g_document || g_document->lineCount == 0) {
        printf("No contacts available.\n");
        return;
    }

    printf("\n! All Contacts !\n");
    size_t contactIndex = 0;
    for (size_t i = 0; i < g_document->lineCount; i++) {
        if (g_document->lines[i].type == DATA_TYPE_CONTACT) {
            printf("%zu: %s %s <%s>\n", contactIndex,
                   g_document->lines[i].data.contact.name,
                   g_document->lines[i].data.contact.surname,
                   g_document->lines[i].data.contact.email);
            contactIndex++;
        }
    }
}

void addContactLine(void) {
    if (!g_document) {
        printf("Error: Document not initialized\n");
        return;
    }

    char name[100], surname[100], email[150];

    printf("Enter name: ");
    if (fgets(name, sizeof(name), stdin) == NULL) {
        printf("Error reading name\n");
        return;
    }
    name[strcspn(name, "\n")] = 0;

    printf("Enter surname: ");
    if (fgets(surname, sizeof(surname), stdin) == NULL) {
        printf("Error reading surname\n");
        return;
    }
    surname[strcspn(surname, "\n")] = 0;

    printf("Enter email: ");
    if (fgets(email, sizeof(email), stdin) == NULL) {
        printf("Error reading email\n");
        return;
    }
    email[strcspn(email, "\n")] = 0;

    ensureContactCapacity(g_document->lineCount + 1);

    LineData* newLine = &g_document->lines[g_document->lineCount];
    newLine->type = DATA_TYPE_CONTACT;

    strncpy(newLine->data.contact.name, name, sizeof(newLine->data.contact.name) - 1);
    newLine->data.contact.name[sizeof(newLine->data.contact.name) - 1] = '\0';

    strncpy(newLine->data.contact.surname, surname, sizeof(newLine->data.contact.surname) - 1);
    newLine->data.contact.surname[sizeof(newLine->data.contact.surname) - 1] = '\0';

    strncpy(newLine->data.contact.email, email, sizeof(newLine->data.contact.email) - 1);
    newLine->data.contact.email[sizeof(newLine->data.contact.email) - 1] = '\0';

    g_document->lineCount++;

    saveContactsToFile();
    printf("Contact added successfully!\n");
}

void editContactLine(void) {
    if (!g_document || g_document->lineCount == 0) {
        printf("No contacts available to edit.\n");
        return;
    }

    printAllContacts();

    size_t contactIndex;
    printf("Enter contact index to edit: ");
    if (scanf("%zu", &contactIndex) != 1) {
        printf("Invalid input\n");
        clearInputBuffer();
        return;
    }
    clearInputBuffer();

    size_t actualIndex = 0;
    size_t currentContactIndex = 0;

    for (size_t i = 0; i < g_document->lineCount; i++) {
        if (g_document->lines[i].type == DATA_TYPE_CONTACT) {
            if (currentContactIndex == contactIndex) {
                actualIndex = i;
                break;
            }
            currentContactIndex++;
        }
    }

    if (currentContactIndex != contactIndex || actualIndex >= g_document->lineCount) {
        printf("Invalid contact index\n");
        return;
    }

    char name[100], surname[100], email[150];

    printf("Current contact: %s %s <%s>\n",
           g_document->lines[actualIndex].data.contact.name,
           g_document->lines[actualIndex].data.contact.surname,
           g_document->lines[actualIndex].data.contact.email);

    printf("Enter new name: ");
    if (fgets(name, sizeof(name), stdin) == NULL) {
        printf("Error reading name\n");
        return;
    }
    name[strcspn(name, "\n")] = 0;

    printf("Enter new surname: ");
    if (fgets(surname, sizeof(surname), stdin) == NULL) {
        printf("Error reading surname\n");
        return;
    }
    surname[strcspn(surname, "\n")] = 0;

    printf("Enter new email: ");
    if (fgets(email, sizeof(email), stdin) == NULL) {
        printf("Error reading email\n");
        return;
    }
    email[strcspn(email, "\n")] = 0;

    strncpy(g_document->lines[actualIndex].data.contact.name, name, sizeof(g_document->lines[actualIndex].data.contact.name) - 1);
    g_document->lines[actualIndex].data.contact.name[sizeof(g_document->lines[actualIndex].data.contact.name) - 1] = '\0';

    strncpy(g_document->lines[actualIndex].data.contact.surname, surname, sizeof(g_document->lines[actualIndex].data.contact.surname) - 1);
    g_document->lines[actualIndex].data.contact.surname[sizeof(g_document->lines[actualIndex].data.contact.surname) - 1] = '\0';

    strncpy(g_document->lines[actualIndex].data.contact.email, email, sizeof(g_document->lines[actualIndex].data.contact.email) - 1);
    g_document->lines[actualIndex].data.contact.email[sizeof(g_document->lines[actualIndex].data.contact.email) - 1] = '\0';

    saveContactsToFile();
    printf("Contact updated successfully!\n");
}

int main() {
    Document* document = createDocument();
    if (!document) {
        printf("Failed to create document. Exiting.\n");
        return 1;
    }

    initDataHandler(document);

    int userOption = -1;
    TextBuffer buffer;

    initializeBuffer(&buffer);
    initHistory();
    saveState(&buffer);

    while (userOption != 0) {
        displayMenu();

        if (scanf("%d", &userOption) != 1) {
            printf("Invalid input. Please enter a number.\n");
            clearInputBuffer();
            continue;
        }
        clearInputBuffer();

        if (userOption == 0) {
            printf("GG\nExiting...\n");
            break;
        }

        processUserOption(userOption, &buffer);
    }

    freeBuffer(&buffer);
    freeDocument(document);
    return 0;
}

void freeDocument(Document* doc) {
    if (doc) {
        if (doc->lines) {
            for (size_t i = 0; i < doc->lineCount; i++) {
                if (doc->lines[i].type == DATA_TYPE_TEXT && doc->lines[i].data.text) {
                    free(doc->lines[i].data.text);
                }
            }
            free(doc->lines);
        }
        free(doc);
    }
}

void displayMenu(void) {
    printf("\n===== Text Editor Menu =====\n");
    printf("1. Append text to the end\n");
    printf("2. Start a new line\n");
    printf("3. Save text to file\n");
    printf("4. Load text from file\n");
    printf("5. Print current text\n");
    printf("6. Insert text at specific position\n");
    printf("7. Search for text\n");
    printf("8. Clear console\n");
    printf("9. Delete text\n");
    printf("10. Undo\n");
    printf("11. Redo\n");
    printf("12. Paste text\n");
    printf("13. Copy text\n");
    printf("14. Insert with replacement\n");
    printf("15. Cut text\n");
    printf("16. Encrypt current text\n");
    printf("17. Decrypt current text\n");
    printf("18. Encrypt text file\n");
    printf("19. Decrypt text file\n");
    printf("20. Save encrypted text\n");
    printf("21. Load encrypted text\n");
    printf("22. Add contact\n");
    printf("23. Edit contact\n");
    printf("24. View all contacts\n");
    printf("0. Exit\n");
    printf("Enter your choice: ");
}

void processUserOption(int userOption, TextBuffer* buffer) {
    switch (userOption) {
        case 1:
            appendText(buffer);
            break;
        case 2:
            addNewLine(buffer);
            break;
        case 3:
            saveToFile(buffer);
            break;
        case 4:
            loadFromFile(buffer);
            break;
        case 5:
            printCurrentText(buffer);
            break;
        case 6:
            insertTextAtPosition(buffer);
            break;
        case 7:
            searchText(buffer);
            break;
        case 8:
            clearConsole();
            break;
        case 9:
            deleteText(buffer);
            break;
        case 10:
            undoCommand(buffer);
            break;
        case 11:
            redoCommand(buffer);
            break;
        case 12:
            pasteText(buffer);
            break;
        case 13:
            copyText(buffer);
            break;
        case 14:
            insertWithReplacement(buffer);
            break;
        case 15:
            cutText(buffer);
            break;
        case 16:
            encryptCurrentText(buffer);
            break;
        case 17:
            decryptCurrentText(buffer);
            break;
        case 18:
            encryptTextFile();
            break;
        case 19:
            decryptTextFile();
            break;
        case 20:
            saveEncryptedText(buffer);
            break;
        case 21:
            loadEncryptedText(buffer);
            break;
        case 22:
            addContactLine();
            break;
        case 23:
            editContactLine();
            break;
        case 24:
            printAllContacts();
            break;
        default:
            printf("Error. Invalid option. Try again\n");
    }
}

void initializeBuffer(TextBuffer* buffer) {
    buffer->content = (char*)malloc(INITIAL_BUFFER_SIZE * sizeof(char));
    if (buffer->content == NULL) {
        fprintf(stderr, "Memory allocation failed. Exiting program.\n");
        exit(EXIT_FAILURE);
    }

    buffer->size = INITIAL_BUFFER_SIZE;
    buffer->used = 0;
    buffer->content[0] = '\0';
}

void resizeBufferIfNeeded(TextBuffer* buffer, size_t additionalSpace) {
    if (buffer->used + additionalSpace + 1 > buffer->size) {
        size_t newSize = buffer->size * 2;
        if (newSize < buffer->used + additionalSpace + 1) {
            newSize = buffer->used + additionalSpace + 1 + INITIAL_BUFFER_SIZE;
        }

        char* newBuffer = (char*)realloc(buffer->content, newSize);
        if (newBuffer == NULL) {
            fprintf(stderr, "Memory reallocation failed. Cannot expand buffer.\n");
            return;
        }

        buffer->content = newBuffer;
        buffer->size = newSize;

        printf("Buffer resized to %zu bytes\n", buffer->size);
    }
}

void freeBuffer(TextBuffer* buffer) {
    if (buffer->content != NULL) {
        free(buffer->content);
        buffer->content = NULL;
        buffer->size = 0;
        buffer->used = 0;
    }
    freeHistory();
}

void appendText(TextBuffer* buffer) {
    saveState(buffer);
    char input[MAX_INPUT_LENGTH];

    printf("Enter text to append: ");
    if (fgets(input, MAX_INPUT_LENGTH, stdin) == NULL) {
        printf("Error reading input.\n");
        return;
    }

    size_t len = strlen(input);
    if (len > 0 && input[len-1] == '\n') {
        input[len-1] = '\0';
        len--;
    }

    resizeBufferIfNeeded(buffer, len);

    strcat(buffer->content, input);
    buffer->used += len;

    printf("Text appended successfully.\n");
}

void addNewLine(TextBuffer* buffer) {
    saveState(buffer);
    resizeBufferIfNeeded(buffer, 1);

    strcat(buffer->content, "\n");
    buffer->used += 1;

    printf("New line started.\n");
}

void saveToFile(TextBuffer* buffer) {
    char filename[MAX_FILENAME_LENGTH];
    FILE* file;

    printf("Enter the file name for saving: ");
    if (fgets(filename, MAX_FILENAME_LENGTH, stdin) == NULL) {
        printf("Error reading filename.\n");
        return;
    }

    size_t len = strlen(filename);
    if (len > 0 && filename[len-1] == '\n') {
        filename[len-1] = '\0';
    }

    file = fopen(filename, "w");
    if (file == NULL) {
        printf("Error: Could not open file %s for writing.\n", filename);
        return;
    }

    if (fputs(buffer->content, file) == EOF) {
        printf("Error: Failed to write to file %s.\n", filename);
        fclose(file);
        return;
    }

    fclose(file);
    printf("Text has been saved successfully to %s.\n", filename);
}

void loadFromFile(TextBuffer* buffer) {
    saveState(buffer);
    char filename[MAX_FILENAME_LENGTH];
    FILE* file;

    printf("Enter the file name for loading: ");
    if (fgets(filename, MAX_FILENAME_LENGTH, stdin) == NULL) {
        printf("Error reading filename.\n");
        return;
    }

    size_t len = strlen(filename);
    if (len > 0 && filename[len-1] == '\n') {
        filename[len-1] = '\0';
    }

    file = fopen(filename, "r");
    if (file == NULL) {
        printf("Error: Could not open file %s for reading.\n", filename);
        return;
    }

    buffer->content[0] = '\0';
    buffer->used = 0;

    int c;
    size_t index = 0;

    while ((c = fgetc(file)) != EOF) {
        if (index + 1 >= buffer->size) {
            resizeBufferIfNeeded(buffer, INITIAL_BUFFER_SIZE);
            if (index + 1 >= buffer->size) {
                printf("Error: Buffer couldn't be resized enough to load file.\n");
                break;
            }
        }

        buffer->content[index++] = (char)c;
    }

    buffer->content[index] = '\0';
    buffer->used = index;

    fclose(file);
    printf("Text has been loaded successfully from %s.\n", filename);
}

void printCurrentText(TextBuffer* buffer) {
    if (buffer->used == 0) {
        printf("The buffer is empty.\n");
        return;
    }

    printf("\n! Current Text Content !\n");
    printf("%s\n", buffer->content);
    printf("! End of Content !\n");
    printf("Text length: %zu characters\n", buffer->used);
}

void insertTextAtPosition(TextBuffer* buffer) {
    saveState(buffer);
    int line, position;
    char input[MAX_INPUT_LENGTH];

    printf("\nCurrent text with line numbers:\n");

    int lineCount = 0;
    int lineStart = 0;
    for (size_t i = 0; i <= buffer->used; i++) {
        if (buffer->content[i] == '\n' || buffer->content[i] == '\0') {
            printf("%d: %.*s\n", lineCount, (int)(i - lineStart), buffer->content + lineStart);
            lineStart = i + 1;
            lineCount++;
        }
    }

    printf("Choose line and index: ");
    if (scanf("%d %d", &line, &position) != 2) {
        printf("Invalid input format. Please enter two numbers.\n");
        clearInputBuffer();
        return;
    }
    clearInputBuffer();

    if (line < 0 || line >= lineCount) {
        printf("Error: Invalid line number. Must be between 0 and %d.\n", lineCount - 1);
        return;
    }

    int actualPos = 0;
    int currentLine = 0;

    while (currentLine < line && buffer->content[actualPos] != '\0') {
        if (buffer->content[actualPos] == '\n') {
            currentLine++;
        }
        actualPos++;
    }

    int lineLength = 0;
    while (buffer->content[actualPos + lineLength] != '\n' &&
           buffer->content[actualPos + lineLength] != '\0') {
        lineLength++;
    }

    if (position < 0 || position > lineLength) {
        printf("Error: Invalid position. Must be between 0 and %d for this line.\n", lineLength);
        return;
    }

    actualPos += position;

    printf("Enter text to insert: ");
    if (fgets(input, MAX_INPUT_LENGTH, stdin) == NULL) {
        printf("Error reading input.\n");
        return;
    }

    size_t len = strlen(input);
    if (len > 0 && input[len-1] == '\n') {
        input[len-1] = '\0';
        len--;
    }

    resizeBufferIfNeeded(buffer, len);

    memmove(buffer->content + actualPos + len,
            buffer->content + actualPos,
            buffer->used - actualPos + 1);

    memcpy(buffer->content + actualPos, input, len);
    buffer->used += len;

    printf("Text inserted successfully.\n");
}

void searchText(TextBuffer* buffer) {
    char searchStr[MAX_INPUT_LENGTH];

    printf("Enter text to search: ");
    if (fgets(searchStr, MAX_INPUT_LENGTH, stdin) == NULL) {
        printf("Error reading input.\n");
        return;
    }

    size_t len = strlen(searchStr);
    if (len > 0 && searchStr[len-1] == '\n') {
        searchStr[len-1] = '\0';
        len--;
    }

    if (len == 0) {
        printf("Search string cannot be empty.\n");
        return;
    }

    char* pos = buffer->content;
    int found = 0;
    int line = 0;
    int linePos = 0;

    printf("\nSearch results for '%s':\n", searchStr);

    while ((pos = strstr(pos, searchStr)) != NULL) {
        line = 0;
        linePos = 0;

        for (char* p = buffer->content; p < pos; p++) {
            if (*p == '\n') {
                line++;
                linePos = 0;
            } else {
                linePos++;
            }
        }

        printf("Text is present in this position: %d %d\n", line, linePos);
        found++;

        pos += len;
    }

    if (found == 0) {
        printf("No matches found for '%s'.\n", searchStr);
    } else {
        printf("Found %d occurrence(s).\n", found);
    }
}

void clearConsole(void) {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
    printf("Console cleared.\n");
}

void clearInputBuffer(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF) { }
}