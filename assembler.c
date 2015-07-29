/////////////////////////////////////////////////////////////////////////////////////////
///// Ella Hazan
////////////////////////////////////////////////////////////////////////////////////////
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

#include "assembler.h"
#include "utils.h"
#include "symbol_table.h"
#include "commands.h"
#include "registers.h"

bool should_compile = true;
symbol_table_t *assembler_data_table = NULL;
symbol_table_t *assembler_extern_table = NULL;
symbol_table_t *assembler_call_table = NULL;
symbol_table_t *assembler_extern_table_data = NULL;
symbol_table_t *assembler_entry_table = NULL;
FILE *data_file = NULL;

//checks if a line is a comment
bool assembler_is_comment(char *line)
{
	return (';' == line[0]);
}

//checks if a line is empty
bool assembler_is_empty_line(char *line)
{
	int i = 0;
	size_t line_legth = strnlen(line, ASSEMBLER_LINE_SIZE);

	for(i = 0; i < line_legth; i++) {
		if(!isspace(line[i])) {
			break;
		}
	}

	return (i == line_legth);
}

//checks if the first field in a line is a symbol
bool assembler_is_first_field_symbol(char *line)
{
	size_t line_size = 0;
	bool deletemiter_found = false;
	int i = 0;

line = trim_white_spaces(line);
   	line_size = strnlen(line, ASSEMBLER_LINE_SIZE);
	for(i = 0; i < line_size; i++) {
		if(SYMBOL_DELEMITER == line[i]) {
			deletemiter_found = true;
			break;
		}
	}

	return deletemiter_found;
}

//checks if a given symbol is valid
bool assembler_is_valid_symbol(char *line)
{
	size_t line_size = 0;
	int i =0;
	line_size = strnlen(line, ASSEMBLER_LINE_SIZE);

	if(line_size < 1) {
		return false;
	}

	if(!isalpha(line[0])) {
		return false;
	}

	for(i = 1; i < line_size; i++) {
		if(SYMBOL_DELEMITER == line[i]) {
			break;
		}

		if(!isalnum(line[i])) {
			return false;
		}
	}

	if(commands_is_valid_command_name(line)) {
		return false;
	}

	if(registers_is_valid_register(line)) {
		return false;
	}

	return true;
}

//returns the symbol from a given line
char *assembler_get_symbol(char *line, char *symbol)
{
	size_t line_size = 0;
	int i =0;

	line = trim_white_spaces(line);
	line_size = strnlen(line, ASSEMBLER_LINE_SIZE);

	/* copy symbol */
	memccpy(symbol, line, SYMBOL_DELEMITER, strnlen(line, line_size));
	symbol[strnlen(symbol, line_size)-1] = '\0'; /* remove trailing delemiter */

	for (i=0; i < line_size; i++) {
		if(SYMBOL_DELEMITER == line[i]) {
			line[i] = '\0';
			break;
		}
	}

	return &(line[i+1]);
}

//skips a word in a line
char *assembler_skip_word(char *line)
{
	size_t line_size = 0;
	int i = 0;

	line = trim_white_spaces(line);
	line_size = strnlen(line, ASSEMBLER_LINE_SIZE);

	for(i=0; i < line_size; i++) {
		if (isspace(line[i])) {
			return &(line[i+1]);
		}
	}

	return NULL; /* never gets here (if you first validate its a directive call */
}

//checks if a line is a .string call
bool assembler_is_string_call(char *line)
{
	if(0 == strncmp(line, ".string", sizeof(".string")-1)) {
		return true;
	}

	return false;
}

//checks if a string call is valid
bool assembler_is_valid_string_call(char *line) {
	line = trim_white_spaces(line);

	if('"' != line[0]) {
		return false;
	}

	if('"' != line[strnlen(line, ASSEMBLER_LINE_SIZE)-1]) {
		return false;
	}

	return true;
}

//checks if  line is a .data call
bool assembler_is_data_call(char *line)
{
	if (0 == strncmp(line, ".data", sizeof(".data")-1)) {
		return true;
	}

	return false;
}

//checks if a data call is valid
bool assembler_is_valid_data_call(char *line) {
	char tmpline[ASSEMBLER_LINE_SIZE] = {0,};
	char *token = NULL;
	int i = 0;
	size_t token_size = 0;

	line = trim_white_spaces(line);
	strncpy(tmpline, line, ASSEMBLER_LINE_SIZE);
	token =	strtok(tmpline, ",");
	while(NULL != token) {
		token = trim_white_spaces(token);
		for(i = 0; i < strlen(token); i++) {
			if(('-' == token[i]) || ('+' ==  token[i])) {
				if(0 != i) {
					printf("+/- are not allow in the middle of a number/n");
					return false;
				}
				continue;
			}

			if(!isdigit(token[i])) {
				printf("only digits are allowd '%c'\n", token[i]);
				return false;
			}
		}
		token = strtok(NULL, ",");
	}

	return true;
}

//adds data to the data table
void assembler_add_to_data_table(char *symbol, int dc)
{
	symbol_table_append(&assembler_data_table, symbol, dc);
}

//adds a string to the data section
void assembler_add_string_to_data_section(char *line, int *dc)
{
	line += 1; /* skip the opening " */
	line[strnlen(line, ASSEMBLER_LINE_SIZE)-1] = '\0'; /* remove the closing " */
	/* the +1 is to include the null termination */
	fwrite(line, strnlen(line, ASSEMBLER_LINE_SIZE)+1, 1, data_file);
	*dc += strnlen(line, ASSEMBLER_LINE_SIZE) + 1;
}

//adds data to the data table
void assembler_add_data_to_data_table(char *line, int *dc) {
	char tmpline[ASSEMBLER_LINE_SIZE] = {0,};
	char *token = NULL;
	int current_number = 0;

	line = trim_white_spaces(line);
	strncpy(tmpline, line, ASSEMBLER_LINE_SIZE);
	token =	strtok(tmpline, ",");
	while(NULL != token) {
		current_number = atoi(token);
		fwrite(((char *)&current_number), 1, 1, data_file);
		*dc += 1;
		token = strtok(NULL, ",");
	}
}

//checks if a line is an .extern call
bool assembler_is_external_call(char *line)
{
	if (0 == strncmp(line, ".extern", sizeof(".extern")-1)) {
		return true;
	}

	return false;
}

//checks if a line is an .entry call
bool assembler_is_entry_call(char *line)
{
	if (0 == strncmp(line, ".entry", sizeof(".entry")-1)) {
		return true;
	}

	return false;
}

//does the first loop logic on the given code
void assembler_first_loop(char *line, int *ic, int *dc)
{
	bool symbol_exists = false;
	char symbol[ASSEMBLER_LINE_SIZE] = {0, };
	int command = -1;
	command_addressing addressings[2] = {INVALID, INVALID, };
	command_arguments arguments_type;

	if(assembler_is_comment(line)) {
		return;
	}

	if(assembler_is_empty_line(line)) {
		return;
	}

	line = trim_white_spaces(line);

	if(assembler_is_first_field_symbol(line)) {
		symbol_exists = true;
		if(!assembler_is_valid_symbol(line)) {
			printf("Invalid label in: %s\n", line);
			should_compile = false;
			return;
		}
		line = assembler_get_symbol(line, symbol);
	}

	line = trim_white_spaces(line);

	if(assembler_is_string_call(line)) {
		line = assembler_skip_word(line);
		if(!assembler_is_valid_string_call(line)) {
			printf("Invalid string call in '%s'\n", line);
			should_compile = false;
			return;
		}

		if(symbol_exists) {
			assembler_add_to_data_table(symbol, *dc);
		}

		assembler_add_string_to_data_section(line, dc);
		return;
	} 

	if(assembler_is_data_call(line)) {
		line = assembler_skip_word(line);
		if(!assembler_is_valid_data_call(line)) {
			printf("Invalid data call in '%s'\n", line);
			should_compile = false;
			return;
		}

		if(symbol_exists) {
			assembler_add_to_data_table(symbol, *dc);
		}

		assembler_add_data_to_data_table(line, dc);
		return;
	}

	if(assembler_is_external_call(line)) {
		line = assembler_skip_word(line);

		if(!assembler_is_valid_symbol(line)) {
			printf("Only valid symbols here!\n");
			should_compile = false;
			return;
		}

		symbol_table_append(&assembler_extern_table, line, 0); /* stub */
		return;
	}

	if(assembler_is_entry_call(line)) {
		line = assembler_skip_word(line);

		/* We should anyway print an error */
		if(!assembler_is_valid_symbol(line)) {
			printf("Only valid symvols here!\n");
			should_compile = false;
			return;
		}

		symbol_table_append(&assembler_entry_table, line, 0); /* stub*/
		
		return;
	}

	/* If we got here, we got a normal assmbly command */
	if(symbol_exists) {
		symbol_table_append(&assembler_call_table, symbol, *ic);
	}

	if(!commands_is_valid_command_name(line)) {
		printf("Invalid command name in '%s'\n", line);
		should_compile = false;
		return;
	}

	command = commands_get_command(line);

	line = assembler_skip_word(line);
	line = trim_white_spaces(line);

	if(!commands_is_valid_addressing(command, line)) {
		should_compile = false;
		return;
	}
	
	arguments_type = commands_get_arguments_type(command);
	switch(arguments_type)
	{
		case TWO:
			commands_get_two_addressings(line, addressings);
			break;
		case ONE:
			addressings[0] = commands_get_one_addressing(line);	
			break;
		case NONE:
			break;
	}

	*ic += commands_get_opcode_size(arguments_type, addressings);
}

//does the second loop logic on the given code
void assembler_second_loop(char *line, FILE *obj_file, int *ic)
{
	int command = -1;
	char symbol[ASSEMBLER_LINE_SIZE] = {0, };
	command_addressing addressings[2] = {INVALID, INVALID, };
	int arguments[2]  = {-1, -1};
	int opcode_size = -1;
	command_arguments arguments_type = -1;

	if(assembler_is_comment(line)) {
		return;
	}

	if(assembler_is_empty_line(line)) {
		return;
	}

	line = trim_white_spaces(line);

	if(assembler_is_first_field_symbol(line)) {
		line = assembler_get_symbol(line, symbol);
	}

	line = trim_white_spaces(line);
	if(assembler_is_entry_call(line) 
			|| assembler_is_data_call(line) 
			|| assembler_is_string_call(line) 
			|| assembler_is_external_call(line)) {
		return;
	}

	line = trim_white_spaces(line);
	command = commands_get_command(line);

	line = assembler_skip_word(line);
	line = trim_white_spaces(line);

	arguments_type = commands_get_arguments_type(command);
	
	switch(arguments_type)
	{
		case TWO:
			commands_get_two_addressings(line, addressings);
			if(!commands_get_two_arguments(line, addressings, arguments, *ic)) {
				printf("Invalid symbols\n");
				should_compile = false;
				return;
			}
					break;
		case ONE:
			addressings[1] = commands_get_one_addressing(line);	
			if(!commands_get_one_argument(line, addressings[1], &(arguments[1]),1+ (*ic))) {
				printf("Invalid symbol\n");
				should_compile = false;
				return;
			}
			break;
		case NONE:
			break;
	}

	if(DIRECT_REGISTER == addressings[0]) {
		arguments[0] = arguments[0] << 5;
		if(DIRECT_REGISTER == addressings[1]) {
			arguments[1] = arguments[1] << 2;
		}

	}

	opcode_size = commands_get_opcode_size(arguments_type, addressings);
	fprintf(obj_file, "%02X ", *ic);
	//write opcode to file
	commands_print_opcode_to_file(obj_file, command, arguments_type, addressings, arguments, *ic);
	*ic += opcode_size;
}

void handle_dollar_sign(char * line) {
  int i,z,j,room,lastSpace=-1;
  bool found = false ;
  static char last[20] = "zevel";

  if (line[0] == '.' || line[0] == '\0') return;
  
  for (i = 0; i < strlen(line);i++) {
    if (line[i] == '$' && line[i+1] == '$') {
      found = true;
      break;
    }
  }

  if (found == true) {
    if (last[0] == '\0') {
      printf("you gave me $$ and I don't know how to replace it...");
      exit(1);
    }
    
    room = strlen(last) - 2;
    
    for(i = strlen(line); i >= 0; i--) {
      if (line[i] == '$') {
        for (z = 0,j = i-1; z < strlen(last); j++,z++)
          line[j] = last[z];
        return;
      } else {
        line[i+room] = line[i];
      }
      
    }
    
  } else {
    for (i = 0; i < strlen(line);i++) {
      if (line[i] == ' ') {
        lastSpace = i;
      } else if (line[i] == ','){
        //for (j=0,z=lastSpace+1; z < (lastSpace  +(i - lastSpace - 1)); z++,j++ )
          //last[j] = line[z];
        //last[j] = '\0';
      }
    }
  }
}

//full proccess a code file, this is the assembler's "main" function
void assembler_proccess_file(FILE *code_file, FILE *obj_file, FILE *entry_file, FILE *ext_file)
{
	int ic = 100;
	int dc = 0;

	char line[ASSEMBLER_LINE_SIZE] = {0, };
	char current_data = 0;
	symbol_table_t *current_table = NULL;
	symbol_table_t *containing_table = NULL;

	if (NULL == code_file) {
		printf("Error, should be given a valid file.\n");
		goto Exit;
	}

	/* First loop */
	while (NULL != fgets(line, ASSEMBLER_LINE_SIZE, code_file)) {
    printf("-> %s\n",trim_white_spaces(line));
    handle_dollar_sign(trim_white_spaces(line));
    printf("-> %s\n",trim_white_spaces(line));
		assembler_first_loop(line, &ic, &dc);
	}

	if (!should_compile) {
		printf("not compiling!\n");	
		return;
	}

	fseek(code_file, 0, SEEK_SET);
	/* adds ic to all data symbols */
	symbol_table_add_to_all_symbols(assembler_data_table, ic);

	ic = 100;

	/* Second loop */
	while (NULL != fgets(line, ASSEMBLER_LINE_SIZE, code_file)) {
		assembler_second_loop(line, obj_file, &ic);
	}

	//write data
	fclose(data_file);
	data_file = fopen("/tmp/data.section", "rb");	
	while(0!=fread(&current_data,1,1,data_file)) {
		fprintf(obj_file, "%X %03X\n", ic, current_data & 0xFFF);
		ic++;
	}
	
	//write extrnal
	current_table = assembler_extern_table_data->previous;
	while(current_table != NULL) {
		fprintf(ext_file, "%s %X\n", current_table->symbol, current_table->counter);
		current_table = current_table->previous;
	}
	
	//write entry
	current_table = assembler_entry_table->previous;
	while(current_table != NULL) {
		//if in: call, data then write
		if(symbol_table_symbol_exists(assembler_data_table, current_table->symbol)) {
			containing_table = assembler_data_table;
		} else if(symbol_table_symbol_exists(assembler_call_table, current_table->symbol)) {
			containing_table = assembler_call_table;
		}
		else {
			printf("No such entry: %s\n", current_table->symbol);
			should_compile = false;
		}

		fprintf(entry_file, "%s %X\n", current_table->symbol, symbol_table_get_value(containing_table, current_table->symbol));
		current_table = current_table->previous;
	}

	if (!should_compile) {
		printf("not compiling!\n");	
		return;
	}
Exit:
	return;
}

//init the assembler, must be called before proccess_file
void assembler_init(void)
{
	assembler_data_table = symbol_table_create();
	data_file = fopen("/tmp/data.section", "wb");	
	assembler_extern_table = symbol_table_create();
	assembler_call_table = symbol_table_create();
	assembler_extern_table_data = symbol_table_create();
	assembler_entry_table = symbol_table_create();
}

//destroy the assembler, must be cakked after proccess_file, or you will leak memory and files
void assembler_destroy(void)
{
	symbol_table_destroy(assembler_data_table);
	symbol_table_destroy(assembler_extern_table);
	symbol_table_destroy(assembler_call_table);
	symbol_table_destroy(assembler_extern_table_data);
	symbol_table_destroy(assembler_entry_table);
	if (NULL != data_file) {
		fclose(data_file);
	}
}

//gets the value of a given symbol
bool assembler_get_symbol_value(char *symbol, int *data, int ic)
{
	symbol_table_t *correct_table = NULL;

	if(!symbol_table_symbol_exists(assembler_data_table, symbol)) {
		if (!symbol_table_symbol_exists(assembler_call_table, symbol)) {
			if(!symbol_table_symbol_exists(assembler_extern_table, symbol)) {
				return false;
			}
			// add value in extern table
			symbol_table_append(&assembler_extern_table_data, symbol, ic);
			*data = 1;
			return true;
		} else {
			correct_table = assembler_call_table;
		}
	} else {
		correct_table = assembler_data_table;
	}


	*data = symbol_table_get_value(correct_table, symbol);

	return true;
}
