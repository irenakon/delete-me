#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <ctype.h>

#include "assembler.h"
#include "commands.h"
#include "utils.h"
#include "registers.h"

command_addressing command_valid_addressing[COMMANDS_AMOUNT][2][4] = {
	{{IMMEDIATE, DIRECT, LAST, DIRECT_REGISTER, }, {DIRECT, DIRECT_REGISTER, INVALID, } },
	{{IMMEDIATE, DIRECT, LAST, DIRECT_REGISTER, }, {IMMEDIATE, DIRECT, LAST, DIRECT_REGISTER, }},
	{{IMMEDIATE, DIRECT, LAST, DIRECT_REGISTER, }, {DIRECT, DIRECT_REGISTER, INVALID, }},
	{{IMMEDIATE, DIRECT, LAST, DIRECT_REGISTER, }, {DIRECT, DIRECT_REGISTER, INVALID, }},
	{{INVALID, }, {DIRECT, DIRECT_REGISTER, INVALID,}},
	{{INVALID, }, {DIRECT, DIRECT_REGISTER, INVALID, }},
	{{DIRECT, INVALID, },{DIRECT, DIRECT_REGISTER, INVALID, }},
	{{INVALID, }, {DIRECT, DIRECT_REGISTER, INVALID,}},
	{{INVALID, }, {DIRECT, DIRECT_REGISTER, INVALID, }},
	{{INVALID, }, {DIRECT, LAST, DIRECT_REGISTER, INVALID, }},
	{{INVALID, }, {DIRECT, LAST, DIRECT_REGISTER, INVALID, }},
	{{INVALID, }, {DIRECT, LAST, DIRECT_REGISTER, INVALID, }},
	{{INVALID, }, {IMMEDIATE, DIRECT, LAST, DIRECT_REGISTER}},
	{{INVALID, }, {DIRECT, INVALID, }},
	{{INVALID, }, {INVALID, }},
	{{INVALID, }, {INVALID, }},
};

char *commands[COMMANDS_AMOUNT] = {
	"mov",
	"cmp",
	"add",
	"sub",
	"not",
	"clr",
	"lea",
	"inc",
	"dec",
	"jmp",
	"bne",
	"red",
	"prn",
	"jsr",
	"rts",
	"stop",
};

//checks if a given command name is valid
bool commands_is_valid_command_name(char *line)
{
	int i =0;

	for(i = 0; i < COMMANDS_AMOUNT; i++) {
		if (0 == strncmp(line, commands[i], strlen(commands[i]))) {
			return true;
		}
	}

	return false;
}

//gets the command id from a command name
int commands_get_command(char *line)
{
	int i =0;

	for(i = 0; i < COMMANDS_AMOUNT; i++) {
		if (0 == strncmp(line, commands[i], strlen(commands[i])-1)) {
			return i;
		}
	}

	return -1;
}

//gets the argument's type of a given command
command_arguments commands_get_arguments_type(int command)
{
	switch(command) {
		case 00:
		case 01:
		case 02:
		case 03:
		case 06:
			return TWO;
		case 04:
		case 05:
		case 07:
		case 010:
		case 011:
		case 012:
		case 013:
		case 014:
		case 015:
			return ONE;
		case 016:
		case 017:
			return NONE;
	}
}

//checks for the command's syntax
bool commands_is_valid_addressing_syntax(char *line, command_addressing addressing)
{
	char tmpline[ASSEMBLER_LINE_SIZE] = {0, };
	char *token = NULL;
	int i = 0;

	line = trim_white_spaces(line);
	strncpy(tmpline, line, strnlen(line, ASSEMBLER_LINE_SIZE));
	switch(addressing) {
	case IMMEDIATE:
		/* the +1 is to skip the first # */
		token = strtok(tmpline + 1, ",");
		if (NULL == token) {
			printf("Invalid addressing in: '%s'\n", line);
		}

		token = trim_white_spaces(token);

		/* if we pass this, then the first value is a valid number */
		for(i = 0; i < strlen(token); i++) {
			if(i==0 && (('-' == token[i]) || '+' == token[i])) {
				continue;
			}

			if(!isdigit(token[i])) {
				printf("Invalid number format in: %s\n", line);
				return false;
			}
		}


		token = strtok(NULL, ",");
		token = trim_white_spaces(token);
		/* second value must be a register */
		if(!registers_is_valid_register(token)) {
			printf("Should be a register in: %s\n", line);
			return false;
		}
		
		return true;
	case DIRECT:
		token = strtok(tmpline, ",");
		token = trim_white_spaces(token);
		if(!assembler_is_valid_symbol(token)) {
			printf("Shoud be a symbol in: %s\n", line);
			return false;
		}

		token = strtok(NULL, ",");
		token = trim_white_spaces(token);
		if(!registers_is_valid_register(token)) {
			printf("Should be a register in: %s\n", line);
			return false;
		}

		return true;
	case DIRECT_REGISTER:
		token = strtok(tmpline, ",");
		token = trim_white_spaces(token);
		if(!registers_is_valid_register(token)) {
			printf("Should be a register in: %s\n", line);
			return false;
		}
		token = strtok(tmpline, ",");
		token = trim_white_spaces(token);
		if(!registers_is_valid_register(token)) {
			printf("Should be a register in: %s\n", line);
			return false;
		}
		return true;
	}

	/* given invalid addressing method */
	return false;
}

//checks the distance addressing syntax
bool commands_check_distance_valid(char *line) {
	char *second = NULL;
	int i = 0;
	if ('(' != line[0]) {
		return false;
	}
	line += 1;

	for(i = 0; i < strlen(line); i++) {
		if(',' == line[i]) {
			line[i] = '\0';
			second = line+i+1;
			break;
		}
	}
	if(NULL == second) {
		return false;
	}
	line = trim_white_spaces(line);
	second = trim_white_spaces(second);

	if(!(assembler_is_valid_symbol(line) && assembler_is_valid_symbol(second))) {
		return false;
	}

	return true;	
}

//gets one addressing type from a line
command_addressing commands_get_one_addressing(char *line) {
	if(('#' == line[0]) && utils_is_number(line+1)) {
		return IMMEDIATE;
	}

	if(assembler_is_valid_symbol(line)) {
		return DIRECT;
	}

	if(registers_is_valid_register(line)) {
		return DIRECT_REGISTER;
	}

	return INVALID;
}

//check if the given addressing type is valid for a command
bool commands_check_one_addressing(int command, int addressing) {
	int i = 0;
	command_addressing *valid_addressings = command_valid_addressing[command][1];
	for(i = 0; i < 4; i++) {
		if(valid_addressings[i] == INVALID) {
			break;
		}

		if(valid_addressings[i] == addressing) {
			return true;
		}
	}


	return false;
}

//gets two addressing type's from a line
void commands_get_two_addressings(char *line, command_addressing *addressings)
{
	char tmpline[ASSEMBLER_LINE_SIZE] = {0,};
	char *token = NULL;
	char *second = NULL;

	strncpy(tmpline, line, ASSEMBLER_LINE_SIZE);
	token = strtok(tmpline, ",");
	second = strtok(NULL, ",");
	token = trim_white_spaces(token);
	second = trim_white_spaces(second);

	addressings[0] = commands_get_one_addressing(token);
	if (addressings[0] == INVALID) {
		addressings[1] == INVALID;
		return;
	}

	addressings[1] = commands_get_one_addressing(second);
}

//checks if the addressing types are valid for a command
bool commands_check_two_addressings(int command, command_addressing *addressings)
{
	int i = 0;
	int j = 0;
	command_addressing *valid_addressings = NULL;
	for(j = 0; j < 2; j++) {
		valid_addressings = command_valid_addressing[command][j];
		for(i = 0; i < 4; i++) {
			if(valid_addressings[i] == INVALID) {
				break;
			}

			if(valid_addressings[i] == addressings[j]) {
				return true;
			}
		}
	}

	return false;
	
}

//completly check if a command line is valid
bool commands_is_valid_addressing(int command, char *line) {
	command_addressing addressing = INVALID;
	command_addressing addressings[2] = {INVALID, INVALID, };
	switch(commands_get_arguments_type(command)) {
		case TWO:
			commands_get_two_addressings(line, addressings);
      if (!commands_check_two_addressings(command, addressings)) {
        printf("bad addressing %s\n",line);
        return false;
      } else {
        return true;
      }
		case ONE:
			addressing = commands_get_one_addressing(line);	
			return commands_check_one_addressing(command, addressing);
		case NONE:
			if(NULL == line) {
				return true; /* valid */
			}
			if(strlen(line) > 0) {
				printf("Shouldn't have arguments: %s\n", line);
				return false;
			}
			return true;
	}

	return false;
}

//gets an argument from a line
bool commands_get_one_argument(char *line, command_addressing addressing, int *data, int ic)
{
	int first_label_value;
	int second_label_value;

	int labels_distence = 0;
	int first_label_ic_distence = 0;
	int second_label_ic_distence = 0;
	char templine[ASSEMBLER_LINE_SIZE] = {0, };
	char *second = NULL;
	int i = 0;
	line = trim_white_spaces(line);
	strncpy(templine, line, strnlen(line, ASSEMBLER_LINE_SIZE));
	switch(addressing) {
		case IMMEDIATE:
			*data = atoi(line + 1);
			*data = *data << 2;
			return true;
		case DIRECT:
			line = trim_white_spaces(line);
	 		return (assembler_get_symbol_value(line, data, ic));
		case DIRECT_REGISTER:
			*data = registers_get_register_value(trim_white_spaces(line)) << 2;
			return true;
	}
	return false;
}

//gets two arguments from a line
bool commands_get_two_arguments(char *line, command_addressing *addressings, int *arguments, int ic)
{
	char tmpline[ASSEMBLER_LINE_SIZE] = {0,};
	char *token = NULL;
	char *second = NULL;

	strncpy(tmpline, line, ASSEMBLER_LINE_SIZE);
  token = strtok(tmpline, ",");
	second = strtok(NULL, ",");

	if(!commands_get_one_argument(token, addressings[0], &(arguments[0]), ic+1)) {
		return false;
	}

	if(!commands_get_one_argument(second, addressings[1], &(arguments[1]), ic+2)) {
		return false;
	}

	return true;
}

//gets the total opcode size
int commands_get_opcode_size(command_arguments arguments_type, command_addressing *addressings)
{
	int size = 1; //the command, and two arguments

	if (ONE == arguments_type) {
		size += 1;
	}

	if(TWO == arguments_type) {
		size += 2;

		if((addressings[0] == DIRECT_REGISTER) && addressings[1] == DIRECT_REGISTER) {
			size -= 1; // here the two arguments enter the same 10bits
		}
	}

	return size;
}

//formats and prints an opcode to a file, should be the .ob file
void commands_print_opcode_to_file(FILE *file, int command, command_arguments arguments_type, command_addressing *addressings, int *arguments, int ic)
{
	int opcode = 0;
	char buf[10] = "";

	opcode = opcode | (arguments_type << 10);
	opcode = opcode | (command << 6);
	
	if(addressings[0] != INVALID) {
		opcode = opcode | (addressings[0] << 4);
	}

	if (addressings[1] != INVALID) {
		opcode = opcode | (addressings[1] << 2);
	}

	//opcode ERA for opcode is always 0
	fprintf(file, "%06d\n", atoi(base4(buf, opcode)));

	if((DIRECT_REGISTER == addressings[0]) && DIRECT_REGISTER == addressings[1]) {
		arguments[0] = arguments[0] | arguments[1];
	}


	if(addressings[0] != INVALID) {
		ic++;
		fprintf(file, "%04d %06d\n", atoi(base4(buf, ic)), 
						atoi(base4(buf, (arguments[0] & 0xFFF))));
	}

	if((DIRECT_REGISTER == addressings[0]) && DIRECT_REGISTER == addressings[1]) {
		return;
	}

	if (addressings[1] != INVALID) {
		ic++;
		fprintf(file, "%04d %06d\n", atoi(base4(buf, ic)), 
					atoi(base4(buf, (arguments[1] & 0xFFF))));
	}
}
