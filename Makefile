all:
	gcc main.c registers.c commands.c assembler.c utils.c symbol_table.c -o assembler
