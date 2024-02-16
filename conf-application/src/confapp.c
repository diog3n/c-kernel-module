#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>

#include "defs.h"
#include "readers.h"
#include "printers.h"

// Checks the string if it's valid name for /proc file. Basically, it 
// checks if the string is alphanumeric and not just numbers, because 
// /proc dir with numeric name will not be created.
// Parameters:
//     char *str - pointer to a null-terminated string to be checked.
// Return value:
//     1 on valid name, 0 on invalid one.
int check_str_valid(char *str);

int main(int argc, char** argv) 
{
	int timer_seconds;

	char procfs_filename[PROCFS_FN_SIZE] = "hellok";

	if (argc < 2) {

		// read the filename interactively, check the result and validate
		// the string
		if (read_procfs_filename_inter(procfs_filename) == ERROR
	     || !check_str_valid(procfs_filename)) {
			print_invalid_filename();
			return EXIT_FAILURE;
		}

		// read the timer parameter interactively and check the result 
		if (read_timer_seconds_inter(&timer_seconds) == ERROR) {
			print_invalid_timer_seconds();
			return EXIT_FAILURE;
		}

	} else {		
		
		// help option flag
		int help_opt = 0;

		// if starts with a dash, it's an option
		if (argc > 1 && argv[1][0] == '-') {
			help_opt = argc == 2 && (strcmp(argv[1], "--help") == 0);
		}

		// if it's a help option, print help
		if (help_opt) {
			print_usage();
			return 0;
		}

		// if it's something else, print error
		if ((argc == 2 && !help_opt) || argc > 3) {
			print_invalid_args();
			return EXIT_FAILURE;
		}

		if (!check_str_valid(argv[argc - 2])) {
			print_invalid_filename();
			return EXIT_FAILURE;
		}

		// procfs_filename is always penultimate.
		// if the name is too long, it will get truncated.
		strncpy(procfs_filename, argv[argc - 2], PROCFS_FN_SIZE - 1);

		// then read an int from argv 
		int read = sscanf(argv[argc - 1], "%d", &timer_seconds);

		if (read == 0 || read == EOF || timer_seconds <= 0) {
			print_invalid_timer_seconds();
			return EXIT_FAILURE;
		}
	}

	// construct a paths from defined strings
	const char *proc_file_name_path_str = PATH_TO_SYS 
	                                  "/" INFTC_MODULE_NAME 
                                      "/" PROC_FILE_NAME_VAR;

	const char *timer_seconds_path_str = PATH_TO_SYS
									 "/" INFTC_MODULE_NAME
									 "/" TIMER_SECONDS_VAR;

	// open file streams in write mode. If user is not
    // root, this will fail and produce NULL pointers
	FILE *proc_file_name_fstream = fopen(proc_file_name_path_str, "w");
	FILE *timer_seconds_fstream  = fopen(timer_seconds_path_str, "w");

	// hence the NULL check
	if (!proc_file_name_fstream || !timer_seconds_fstream) {
		printf("Could not open files. Are you root?\n");
		return EXIT_FAILURE;
	}

	fprintf(proc_file_name_fstream, "%s", procfs_filename);
	fprintf(timer_seconds_fstream, "%d", timer_seconds);

	// close streams in the end
	fclose(proc_file_name_fstream);
	fclose(timer_seconds_fstream);

	return EXIT_SUCCESS;
}

int check_str_valid(char *str) 
{
	// iterate over characters
	for (int i = 0; str[i] != '\0'; i++) {
		
		// if a letter is encountered - it's valid
		if (isalpha(str[i])) {
			return 1;
		}
	}

	// otherwise return 0
	return 0;
}