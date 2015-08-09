#include <ctype.h>
#include <stdbool.h>
#include <string.h>

#include "utils.h"
#include "assembler.h"

//trims a line from all white spaces
char *trim_white_spaces(char *line)
{
  size_t line_length = 0;
  int i = 0;
  char *new_line = line;

  if(NULL == line) {
    return NULL;
  }

  line_length = strnlen(line, ASSEMBLER_LINE_SIZE); 

  for (i = line_length - 1; i >= 0; i--) {
    if(!isspace(line[i])) {
      line[i+1] = '\0';
      break;
    }
  }

  line_length = strnlen(line, ASSEMBLER_LINE_SIZE);
  for(i = 0; i < line_length; i++) {
    if(isspace(line[i])) {
      continue;
    }

    new_line = &(line[i]);
    break;
  }

  return new_line;
}

//checks if a string can be convertef to a number using atoi()
bool utils_is_number(char *line)
{
  size_t l= 0;
  int i = 0;

  l = strnlen(line, ASSEMBLER_LINE_SIZE);
  for(i = 0; i < l; i++) {
    if(i==0 && (('-' == line[0]) || '+' == line[0])) {
      continue;
    }

    if(!isdigit(line[i])) {
      return false;
    }
  }

  return true;
}

char * base4(char * output,long int number_to_convert) {
  char base_digits[16] =
    {'0', '1', '2', '3', '4', '5', '6', '7',
  '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};
  int converted_number[64];
  int next_digit, base=4, index=0,i;

  while (number_to_convert != 0) {
    converted_number[index] = number_to_convert % base;
    number_to_convert = number_to_convert / base;
    ++index;
  }

  --index;
  
  for (i = 0; index >=0;index--,i++) {
    output[i] = base_digits[converted_number[index]];
  }
  output[i] = '\0';
  return output;
}

