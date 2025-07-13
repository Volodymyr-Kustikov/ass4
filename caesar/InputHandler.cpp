#include "InputHandler.h"
#include <limits>

int InputHandler::getIntegerInput(const std::string& prompt) {
    int value;
    std::cout << prompt;

    while (!(std::cin >> value)) {
        std::cout << "Invalid key format." << std::endl;
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << prompt;
    }

    std::cin.ignore();
    return value;
}

std::string InputHandler::getStringInput(const std::string& prompt) {
    std::string input;
    std::cout << prompt;
    std::getline(std::cin, input);
    return input;
}

bool InputHandler::validateInput() {
    return std::cin.good();
}

void InputHandler::clearInputBuffer() {
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}