//
// Created by astesh on 30.09.22.
//

/*
 * На стандартном потоке ввода подается последовательность символов в кодировке UTF-8.

Посчитать количество символов, которые могут быть представлены в кодирове ASCII, и количество символов, которые не могут быть представлены в этой кодировке.

На стандартный поток вывода вывести последовательно два эти числа.

В случае ошибки декодирования потока данных, необходимо вывести результаты подсчёта, полученные на момент обнаружения ошибки, и завершить работу программы с кодом ошибки 1.

Запрещается использовать какие-либо функции
*/

#include <stdio.h>

int main() {
    const int first_bit = 0x80;
    const int first_four_bits = 0xF0;
    const int first_three_bits = 0xE0;
    const int first_two_bits = 0xC0;

    int read = 0;
    int unread = 0;
    char sym;
    while ((sym = getchar()) != EOF) {
        if (sym != EOF) {
            if ((sym & first_bit) == 0) {
                read++;
            } else {
                int length = 0;
                if ((sym & first_four_bits) == first_four_bits) {
                    length = 3;
                } else if ((sym & first_three_bits) == first_three_bits) {
                    length = 2;
                }
                if ((sym & first_two_bits) == first_two_bits) {
                    length = 1;
                }
                for (int i = 0; i < length; i++) {
                    sym = getchar();
                    if (sym == '\0' || (sym & first_two_bits) != first_bit || sym == EOF) {
                        printf("%d %d", read, unread);
                        return 1;
                    }
                }
                if (length) {
                    unread++;
                }
            }
        }
    }
    printf("%d %d", read, unread);
    return 0;
}
