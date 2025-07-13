
#ifdef _WIN32
#define EXPORT __declspec(dllexport)
#else
#define EXPORT
#endif

EXPORT void caesar_encrypt(const char* input, char* output, int key, int length) {
    if (!input || !output || length <= 0) return;

    key = ((key % 26) + 26) % 26;

    for (int i = 0; i < length; i++) {
        char c = input[i];

        if (c >= 'A' && c <= 'Z') {
            output[i] = ((c - 'A' + key) % 26) + 'A';
        } else if (c >= 'a' && c <= 'z') {
            output[i] = ((c - 'a' + key) % 26) + 'a';
        } else {
            output[i] = c;
        }
    }
}

EXPORT void caesar_decrypt(const char* input, char* output, int key, int length) {
    if (!input || !output || length <= 0) return;

    caesar_encrypt(input, output, -key, length);
}

#ifdef _WIN32
#include <windows.h>

    BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
        switch (ul_reason_for_call) {
            case DLL_PROCESS_ATTACH:
            case DLL_THREAD_ATTACH:
            case DLL_THREAD_DETACH:
            case DLL_PROCESS_DETACH:
                break;
        }
        return TRUE;
    }
#endif