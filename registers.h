#ifndef REGISTERS_H
#define REGISTERS_H

#include <stdbool.h>

#define REGISTERS_AMOUNT (11)

char *registers[REGISTERS_AMOUNT];

bool registers_is_valid_register(char *name);
int registers_get_register_value(char *name);

#endif
