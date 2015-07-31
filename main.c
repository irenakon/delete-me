/////////////////////////////////////////////////////////////////////////////////////////
///// Ella Hazan
////////////////////////////////////////////////////////////////////////////////////////
#define FILE_ARGUMENT (1)
#define ARGUMENTS_COUNT (1)

#include <stdio.h>
#include <stddef.h>
#include <string.h>

#include "assembler.h"

void handle_dollar_sign(char * line) {
  int i,z,j,y,room,lastSpace=-1;
  bool found = false ;
  static char last[20];

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
      if (line[i] == '.') return;
      if (line[i] == ' ' && line[i+1] !=' ') {
      lastSpace = i;
      } else if (line[i] == ',' || i == strlen(line)-1){
        y = (i - lastSpace - 1);
        if (strlen(line)-1==i) y++;
        for (j=0,z=lastSpace+1; j < y; z++,j++ ) {
          last[j] = line[z];
        }
        last[j] = '\0';
        return;
      }
    }
  }
}

// preprocess the file, remove dollar signs
FILE * preprocess(char * filename) {
  FILE *writer = NULL;
  FILE *reader = NULL;
  char line[ASSEMBLER_LINE_SIZE];
  
  reader = fopen(filename, "r");
  writer = fopen("/tmp/shit.as","w+");
  
	while (NULL != fgets(line, ASSEMBLER_LINE_SIZE, reader)) {
    handle_dollar_sign(line);
    fputs(line,reader);
  }
  
  fclose(reader);
  rewind(writer);
  
  return writer;
}

//the start point, parse arguments and call assembler
int main(int argc, char *argv[])
{
	FILE *code_file = NULL;
	FILE *obj_file = NULL;
	FILE *ext_file = NULL;
	FILE *entry_file = NULL;

	int i = 0;
	char temp[100] = {0, };
	int len = 0;
	
	if ((argc - 1) < ARGUMENTS_COUNT)
		printf("Usage: %s file\n", argv[0]);
	else {
		for(i = 1; i < argc; i++) {
			//code_file = fopen(argv[i], "r");
			code_file = preprocess(argv[i]);
			memccpy(temp, argv[i], '.', 100);
			len =  strlen(temp) - 1;
			strcpy(temp+len, ".ob");
			obj_file = fopen(temp, "w");
			strcpy(temp+len, ".ent");
			entry_file = fopen(temp, "w");
			strcpy(temp+len, ".ext");
			ext_file = fopen(temp, "w");
			if (NULL == code_file
					|| NULL == obj_file
					|| NULL == entry_file) {
				printf("Unable to open file.\n");
			}
			else {
				assembler_init();
				assembler_proccess_file(code_file, obj_file, entry_file, ext_file);
				assembler_destroy();
			}
			fclose(code_file);
			fclose(obj_file);
			fclose(entry_file);
			fclose(ext_file);
		}
	}

	return 0;
}
