# simulation/alu-sum

Реализуйте функцию с прототипом:

```
#include <stdint.h>

extern void sum(ITYPE first, ITYPE second, ITYPE *res, int *CF);
``` 

в которой ITYPE - это произвольный беззнаковый целочисленный тип данных с неизвестной разрядностью.

Функция должна вычислить сумму значений first и second, результат записать по указателю в выходной параметр res. При
возникновении целочисленного переполнения необходимо в выходной параметр CF записать значение 1, в противном случае -
значение 0.

Используйте побитовые операции для вычислений. Операции сложения и вычитания допускаются только применительно к
переменным цикла.

На сервер необходимо отправить исходный файл, содержащий только реализацию требуемой функции. Для успешной компиляции и
тестирования можно использовать следующий ключ компиляции, задающий требуемый тип: -DITYPE=uint64_t.
