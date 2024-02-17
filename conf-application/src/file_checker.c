#include <dirent.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "defs.h"
#include "file_checker.h"

f_check_res_t check_proc_file_exists(const char *proc_file_name) {
	FILE *file;

	// first, we'll have to construct full file name
	// + 1 is for the slash between strings
	char *path_to_file = malloc(sizeof(PATH_TO_PROC) + strlen(proc_file_name) + 1);

	strcpy(path_to_file, PATH_TO_PROC);

	// appending a slash at the end of the string
	LAST_ELEMENT(path_to_file, sizeof(PATH_TO_PROC)) = '/';
	LAST_ELEMENT(path_to_file, sizeof(PATH_TO_PROC) + 1) = '\0';

	// finally, appending the file name
	strcat(path_to_file, proc_file_name);

	file = fopen(path_to_file, "r");

	free(path_to_file);
	
	if (!file) return FILE_DOES_NOT_EXIST;

	fclose(file);

	return FILE_EXISTS;
}