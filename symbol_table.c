#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "symbol_table.h"

//create a symbol table, call symbol_table_destroy!!!!
symbol_table_t *symbol_table_create(void)
{
	symbol_table_t *table = malloc(sizeof(symbol_table_t));
	if (NULL == table) {
		printf("memory allocation failed.\n");
		exit(1);
	}

	return table;
}

//destroies a symbol table, only call this function, or you will leak memory
void symbol_table_destroy(symbol_table_t *table)
{
	if (NULL != table) {
		if (NULL != table->previous) {
			symbol_table_destroy(table->previous);
		}
		free(table);
	}
}

//appends a symbol to the symbol table
void symbol_table_append(symbol_table_t **table, char *symbol, int counter)
{
	symbol_table_t *next = NULL;
	strncpy((*table)->symbol, symbol, ASSEMBLER_LINE_SIZE);
	(*table)->counter = counter;

	next = malloc(sizeof(symbol_table_t));
	if (NULL == next) {
		printf("Memory allocation failed.\n");
		exit(1);
	}

	next->previous = *table;
	*table = next;
}

//checks if a symbol exists in a symbol table
bool symbol_table_symbol_exists(symbol_table_t *table, char *symbol) 
{
	if(0 == strcmp(table->symbol, symbol)) {
		return true;
	}

	if (table->previous == NULL) {
		return false;
	}

	return symbol_table_symbol_exists(table->previous, symbol);
}

// gets the value of a symbol from the symbol table
int symbol_table_get_value(symbol_table_t *table, char *symbol)
{
	if(0 == strcmp(table->symbol, symbol)) {
		return table->counter;
	}

	if (table->previous == NULL) {
		return -1;
	}

	return symbol_table_get_value(table->previous, symbol);
}

//adds a value to all symbols in a table
void symbol_table_add_to_all_symbols(symbol_table_t *table, int value)
{
	table->counter += value;
	if(table->previous != NULL) {
		symbol_table_add_to_all_symbols(table->previous, value);
	}
}

//changes a value of a symbol in the table
void symbol_table_change_counter(symbol_table_t *table, char *symbol, int value)
{
	if(0 == strcmp(table->symbol, symbol)) {
		table->counter = value;
		return;
	}

	if(table->previous == NULL) {
		return;
	}

	symbol_table_change_counter(table->previous, symbol, value);
}
