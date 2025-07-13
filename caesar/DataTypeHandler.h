#ifndef DATA_TYPE_HANDLER_H
#define DATA_TYPE_HANDLER_H

#include <string>
#include <vector>
#include "../main.h"

class DataTypeHandler {
private:
    Document* document;

public:
    DataTypeHandler(Document* doc);
    ~DataTypeHandler();

    void addTextLine(const std::string& text);
    void addContactLine(const std::string& name, const std::string& surname, const std::string& email);
    void addChecklistLine(const std::string& info, bool checked = false);
    bool editTextLine(size_t lineIndex, const std::string& newText);
    bool editContactLine(size_t lineIndex, const std::string& name, const std::string& surname, const std::string& email);
    bool editChecklistLine(size_t lineIndex, const std::string& info, bool checked);
    bool toggleChecklistItem(size_t lineIndex);
    bool deleteLine(size_t lineIndex);
    std::vector<char> serializeDocument();
    bool deserializeDocument(const std::vector<char>& data);
    void printDocument();
    void printLine(size_t lineIndex);
    std::vector<size_t> searchInDocument(const std::string& searchText);
    bool isValidLineIndex(size_t lineIndex) const;
    DataType getLineType(size_t lineIndex) const;
private:
    void ensureCapacity(size_t requiredCapacity);
    void freeLine(size_t lineIndex);
    std::string serializeLine(size_t lineIndex);
    bool deserializeLine(const std::string& data, size_t lineIndex);
};

#endif