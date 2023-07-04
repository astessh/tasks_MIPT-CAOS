#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define NumberCount 10
#define AlphabetSize 26
#define MaxInput 1000

uint64_t code(const char *a, int start, int ac_point) {
    uint64_t res = 0;
    for (int i = start; i < ac_point; i++) {
        uint64_t temp = 0;
        char el = a[i];
        if (el >= '0' && el <= '9') {
            temp += 1;
            temp <<= el - '0';
        } else if (el >= 'A' && el <= 'Z') {
            temp += 1;
            temp <<= el - 'A' + NumberCount;
        } else if (el >= 'a' && el <= 'z') {
            temp += 1;
            temp <<= el - 'a' + NumberCount + AlphabetSize;
        }
        res |= temp;
    }
    return res;
}

void decode(uint64_t a) {
    for (int i = 0; i < NumberCount + AlphabetSize * 2; i++) {
        if ((a & 1) == 1) {
            if (i >= 0 && i <= NumberCount - 1) {
                printf("%c", i + '0');
            } else if (i >= NumberCount && i <= AlphabetSize + NumberCount - 1) {
                printf("%c", (i - NumberCount) + 'A');
            } else {
                printf("%c", (i - NumberCount - AlphabetSize) + 'a');
            }
        }
        a >>= 1;
    }
}

int main() {
    char *accum = (char *) malloc(MaxInput);
    char sym;
    uint64_t all = 0;
    fgets(accum, MaxInput, stdin);
    int prev = 0;
    for (int i = 0; i < MaxInput; i++) {
        sym = accum[i];
        if (sym == '^' || sym == '~' || sym == '&' || sym == '|') {
            if (sym == '~') {
                all = ~all;
            } else if (sym == '^') {
                all ^= code(accum, prev, i);
            } else if (sym == '|') {
                all |= code(accum, prev, i);
            } else {
                all &= code(accum, prev, i);
            }
            prev = i + 1;
        }
    }
    decode(all);
    free(accum);
    return 0;
}
