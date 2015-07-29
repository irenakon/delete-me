#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#include "assembler.h"

typedef struct symbol_table{
	struct symbol_table *previous;
	char symbol[ASSEMBLER_LINE_SIZE];
	int counter;
} symbol_table_t;


symbol_table_t *symbol_table_create(void);
void symbol_table_destroy(symbol_table_t *table);
void symbol_table_append(symbol_table_t **table, char *symbol, int counter);
bool symbol_table_symbol_exists(symbol_table_t *table, char *symbol);
int symbol_table_get_value(symbol_table_t *table, char *symbol);
void symbol_table_add_to_all_symbols(symbol_table_t *table, int value);
void symbol_table_change_counter(symbol_table_t *table,char *symbol, int value);

#endif 
