#ifndef ASSEMBLER_H
#define ASSEMBLER_H

#define ASSEMBLER_LINE_SIZE (1024)
#define SYMBOL_DELEMITER (':')

#include <stdio.h>
#include <stdbool.h>

void assembler_init(void);
void assembler_destroy(void);
void assembler_proccess_file(FILE *code_file, FILE *obj_file, FILE *entry_file, FILE *ext_file);
bool assembler_is_valid_symbol(char *line);
bool assembler_get_symbol_value(char *symbol, int *data, int ic);
#endif
