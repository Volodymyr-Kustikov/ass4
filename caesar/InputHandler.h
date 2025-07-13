#ifndef INPUTHANDLER_H
#define INPUTHANDLER_H

#include <string>
#include <iostream>

class InputHandler {
public:
    static int getIntegerInput(const std::string& prompt);
    static std::string getStringInput(const std::string& prompt);
    static bool validateInput();
    static void clearInputBuffer();
};

#endif
