#include <windows.h>
#include <stdio.h>
#include <stdint.h>
#include <handleapi.h>

int Execute(HANDLE in) {
    if (in == INVALID_HANDLE_VALUE) {
        return 1;
    }
    int value = 0;
    long next_pointer = 0;
    unsigned long num = 0;
    ReadFile(in, &value, sizeof(value), &num, NULL);
    if (num == 0) {
        return 0;
    }
    ReadFile(in, &next_pointer, sizeof(next_pointer), NULL, NULL);
    while (next_pointer != 0) {
        printf("%d ", value);
        SetFilePointer(in, next_pointer, NULL, FILE_BEGIN);
        ReadFile(in, &value, sizeof(value), NULL, NULL);
        ReadFile(in, &next_pointer, sizeof(next_pointer), NULL, NULL);
    }
    printf("%d ", value);
    return 0;
}

int main(int args, char* argv[]) {
    int dwShareMode = 0;
    HANDLE file = CreateFileA(argv[1], GENERIC_READ, dwShareMode, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_READONLY, NULL);
    int exit_code = Execute(file);
    CloseHandle(file);
    return exit_code;
}
