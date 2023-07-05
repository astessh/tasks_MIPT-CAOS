#include <stdio.h>

int main() {
  char* strs = "#some_string_value #some another value with spaces	";
  int nums = +5 + 7;
  int i = 0;
  while (strs[i] != '\t') {
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
