#ifndef COMMANDS_H
#define COMMANDS_H

#include <stdbool.h>

#define COMMANDS_AMOUNT (16)

char *commands[COMMANDS_AMOUNT];

typedef enum {
	IMMEDIATE=0,
	DIRECT=1,
	DISTENCE=2,
	DIRECT_REGISTER=3,
	INVALID,
} command_addressing;
command_addressing command_valid_addressing[COMMANDS_AMOUNT][2][4];

typedef enum {
	NONE = 0,
	ONE = 1,
	TWO = 2,
} command_arguments;


bool commands_is_valid_command_name(char *line);
int commands_get_command(char *line);
command_arguments commands_get_arguments_type(int command);
bool commands_is_valid_addressing_syntax(char *line, command_addressing addressing);
bool commands_is_valid_addressing(int command, char *line);
void commands_get_two_addressings(char *line, command_addressing *addressings);
command_addressing commands_get_one_addressing(char *line);
bool commands_get_two_arguments(char *line, command_addressing *addressings, int *arguments, int ic);
bool commands_get_one_argument(char *line, command_addressing addressing, int *data, int ic);
int commands_get_opcode_size(command_arguments arguments_type,command_addressing *addressings);
void commands_print_opcode_to_file(FILE *file, int command, command_arguments arguments_type, command_addressing *addressings, int *arguments, int ic);
#endif
