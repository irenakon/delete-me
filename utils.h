#ifndef UTILS_H
#define UTILS_H

#include <stdbool.h>

char *trim_white_spaces(char *line);
bool utils_is_number(char *line);
char * base4(char * output,long int number_to_convert);

#endif
