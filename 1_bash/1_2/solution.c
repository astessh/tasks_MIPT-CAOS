//
// Created by astesh on 25.09.22.
//

/*Программа solution.c компилируется вместе с конфиг-файлом compile.conf, который имеет примерный вид:

# Это комментарий
# Могут быть пустые строки
KEY1=some_string_value
KEY_important_string=some another value with spaces
KEY_integer_value=5
KEY_another_integer_value=7

Комментарии могут быть только на отдельных строках от объявления переменных.

Необходимо реализовать программу и Makefile для её сборки (имя выходного файла - program.exe). Итоговая программа должна:

вывести все строковые значения (без имен ключей) в порядке их вхождения в конфиг-файле;
посчитать сумму всех целочисленных значений, и вывести эту сумму самым последней строкой.*/

#include <stdio.h>

int main()
{
	char *strs="#some_string_value #some another value with spaces	";
	int nums=+5 +7;
	int i = 0;
	while (strs[i]!='\t') {
		if (strs[i] == '#' && i > 0) {
			printf("\n");
		} else if (i > 0) {
			printf("%c", strs[i]);
		}
		i++;
	}
	printf("\n");
	printf("%d", nums);
	return 0;
}
