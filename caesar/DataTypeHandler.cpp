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
    bool editTextLine(size_t lineIndex, const std::string& newText);

    void addChecklistLine(const std::string& info, bool checked = false);
    bool editChecklistLine(size_t lineIndex, const std::string& info, bool checked);
    bool toggleChecklistItem(size_t lineIndex);

    bool deleteLine(size_t lineIndex);
    void printDocument();
    void printLine(size_t lineIndex);
    std::vector<size_t> searchInDocument(const std::string& searchText);

    bool isValidLineIndex(size_t lineIndex) const;
    DataType getLineType(size_t lineIndex) const;

private:
    void ensureCapacity(size_t requiredCapacity);
    void freeLine(size_t lineIndex);
};

#endif