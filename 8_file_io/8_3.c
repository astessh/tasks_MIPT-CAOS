/*Задача, аналогичная print-list-posix, но требуется решение под Windows.

Программе в аргументе командной строки передается имя файла с бинарными данными в Little-Endian.

Файл хранит внутри себя односвязный список элементов:

struct Item
{
  int value;
  uint32_t next_pointer;
};

Поле value храние значение элемента списка, поле next_pointer - позицию в файле (в байтах), указывающую на следующий элемент. Признаком последнего элемента является значение next_pointer, равное 0.

Расположение первого элемента списка (если он существует) - строго в нулевой позиции в файле, расположение остальных - случайным образом.

Выведите на экран значения элементов в списке в текстовом представлении.

Для работы с файлом использовать только низкоуровневый ввод-вывод WinAPI.*/

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
