cmake_minimum_required(VERSION 3.10)
project(TextEditor)

set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 11)

include_directories(${PROJECT_SOURCE_DIR})
include_directories(${PROJECT_SOURCE_DIR}/caesar)
include_directories(${PROJECT_SOURCE_DIR}/cpp)

add_library(caesar SHARED
        caesar/caesar_dll.c
        caesar/InputHandler.h
        caesar/InputHandler.cpp
        caesar/FileManager.cpp
        caesar/FileManager.h
        caesar/CipherManager.cpp
        caesar/CipherManager.h
        caesar/CryptographyService.cpp
        caesar/CryptographyService.h
        caesar/CaesarCipher.cpp     # Add this line
        cpp/additionalFunctionallity.cpp
)

add_executable(text_editor
        main.c
        cpp/additionalFunctionallity.cpp
        caesar/CaesarCipher.cpp
        caesar/DataTypeHandler.cpp
)

target_link_libraries(text_editor caesar dl)
