#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>

#include "defs.h"
#include "readers.h"
#include "printers.h"
#include "file_checker.h"

// Checks the string if it's valid name for /proc file. Basically, it 
// checks if the string is alphanumeric and not just numbers, because 
// /proc dir with numeric name will not be created.
// Parameters:
//     char *str - pointer to a null-terminated string to be checked.
// Return value:
//     SUCCESS on valid name, FAILURE on invalid one.
int check_str_valid(char *str);

int main(int argc, char** argv) 
{
	int timer_seconds;

	char procfs_filename[PROCFS_FN_SIZE + 1] = "hellok";

	if (argc < 2) {
		read_status_t read_filename_result = 
							read_procfs_filename_inter(procfs_filename);

		// read the filename interactively, check the result and validate
		// the string
		if (read_filename_result == READ_NONE) {
			print_invalid_filename();
			return EXIT_FAILURE;
		}

		if (read_filename_result == READ_OVERFLOW) {
			print_name_too_long();
			return EXIT_FAILURE;
		}

		read_status_t read_timer_seconds_result = 
					           read_timer_seconds_inter(&timer_seconds);

		// read the timer parameter interactively and check the result 
		if (read_timer_seconds_result == READ_NONE) {
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

		// if it's something else, or too many arguments print error
		if ((argc == 2 && !help_opt) || argc > 3) {
			print_invalid_args();
			return EXIT_FAILURE;
		}

		// if name is too long, exit
		if (strlen(argv[argc - 2]) > PROCFS_FN_SIZE) {
			print_name_too_long();
			return EXIT_FAILURE;
		}

		// procfs_filename is always penultimate.
		strncpy(procfs_filename, argv[argc - 2], PROCFS_FN_SIZE - 1);

		// then read an int from argv 
		int read = sscanf(argv[argc - 1], "%d", &timer_seconds);

		if (read == 0 || read == EOF) {
			print_invalid_timer_seconds();
			return EXIT_FAILURE;
		}
	}

	// Final checks

	// If string is numeric, then it is not a valid file name
	if (!(check_str_valid(procfs_filename))) {
		print_invalid_filename();
		return EXIT_FAILURE;
	}


	// Finally, if the timer is negative or equal to zero, we do not
	// allow it.
	if (timer_seconds <= 0) {
		print_invalid_timer_seconds();
		return EXIT_FAILURE;
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
	FILE *timer_seconds_fstream  = fopen(timer_seconds_path_str, "w");

	if (!timer_seconds_fstream) {
		printf("Could not open files. Are you root?\n");
		return EXIT_FAILURE;
	}

	fprintf(timer_seconds_fstream, "%d", timer_seconds);
	fclose(timer_seconds_fstream);

	// If a file or directory with such name already exists, then
	// it should not be created. Instead, only timer_seconds will
	// be changed.
	// Actually (c), the file will not be created anyway, since 
	// kernel does not register /proc entries with equal names,
	// but to avoid some really nasty error messages in system logs,
	// I decided to implement this protection mechanism.  
	if (check_proc_file_exists(procfs_filename) == FILE_EXISTS) {
		print_file_already_exists();
		return EXIT_SUCCESS;
	}
			
	FILE *proc_file_name_fstream = fopen(proc_file_name_path_str, "w");

	if (!timer_seconds_fstream) {
		printf("Could not open files. Are you root?\n");
		return EXIT_FAILURE;
	}

	fprintf(proc_file_name_fstream, "%s", procfs_filename);

	// close streams in the end
	fclose(proc_file_name_fstream);

	printf("Done!\n");
	return EXIT_SUCCESS;
}

int check_str_valid(char *str) 
{
	// iterate over characters
	for (int i = 0; str[i] != '\0'; i++) {
		
		// if a letter is encountered - it's valid
		if (isalpha(str[i])) {
			return SUCCESS;
		}
	}

	// otherwise return 0
	return FAILURE;
}