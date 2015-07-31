#define FILE_ARGUMENT (1)
#define ARGUMENTS_COUNT (1)

#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>

#include "assembler.h"

// preprocess the file, remove dollar signs
FILE * preprocess(char * filename) {
  FILE *writer = NULL;
  FILE *reader = NULL;
  char line[ASSEMBLER_LINE_SIZE];
  char tmp[ASSEMBLER_LINE_SIZE];
  
  reader = fopen(filename, "r");
  writer = fopen("/tmp/temp.as","w+");
  
  if (reader == NULL) {
    printf("Can't find file: %s\n",filename);
    exit(1);
  }
  
  while (NULL != fgets(line, ASSEMBLER_LINE_SIZE, reader)) {
    trim_white_spaces(line);
    change_dollar_sign(line);
    sprintf(tmp,"%s\n",line);
    fputs(tmp,writer);
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
      if (rindex(argv[i],'.') == NULL)
        sprintf(temp,"%s.as",argv[i]);
       else
        sprintf(temp,"%s",argv[i]);
      
			code_file = preprocess(temp);
			memccpy(temp, temp, '.', 100);
			len =  strlen(temp) - 1;
			strcpy(temp+len, ".ob");
			obj_file = fopen(temp, "w");
			strcpy(temp+len, ".ent");
			entry_file = fopen(temp, "w");
			strcpy(temp+len, ".ext");
			ext_file = fopen(temp, "w");
      
			if ( NULL == obj_file || NULL == entry_file) 
				printf("Unable to open file.\n");
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
