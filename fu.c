#include <stdio.h>

int main() {
  int ic = 14;
  int current_data = 9;
  printf("%X %03X\n", ic, current_data & 0xFFF);
  return 0;
}
