/*Реализуйте функцию с прототипом:

#include <stdint.h>

extern void sum(ITYPE first, ITYPE second, ITYPE *res, int *CF);


в которой ITYPE - это произвольный беззнаковый целочисленный тип данных с неизвестной разрядностью.

Функция должна вычислить сумму значений first и second, результат записать по указателю в выходной параметр res. При возникновении целочисленного переполнения необходимо в выходной параметр CF записать значение 1, в противном случае - значение 0.

Используйте побитовые операции для вычислений. Операции сложения и вычитания допускаются только применительно к переменным цикла.*/
#include <stdint.h>

extern void sum(ITYPE first, ITYPE second, ITYPE* res, int* CF) {
  ITYPE carry = 0;
  ITYPE size = sizeof(ITYPE);
  for (ITYPE i = 0; i < size; ++i) {
    carry = first & second;
    first = first ^ second;
    second = (carry << 1);
  }

  *res = first;
  if (carry == 0) {
    *CF = 0;
  } else {
    *CF = 1;
  }
}
