#include <stdbool.h>
#include <string.h>
#include <stdlib.h>

#include "registers.h"

char *registers[REGISTERS_AMOUNT] = {
	"r0",
	"r1",
	"r2",
	"r3",
	"r4",
	"r5",
	"r6",
	"r7",
	"pc",
	"sp",
	"psw",
};

//checks if a register name is valid
bool registers_is_valid_register(char *name) 
{
	int i =0;

	for(i = 0; i < REGISTERS_AMOUNT; i++) {
		if (0 == strncmp(name, registers[i], strlen(registers[i])-1)) {
			return true;
		}
	}

	return false;
}

//gets a register's value
int registers_get_register_value(char *name)
{
	return atoi(name + 1);
}
