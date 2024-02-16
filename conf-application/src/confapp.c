#include <ctype.h>
#include <string.h>
#include <stdio.h>

#define INFTC_MODULE_NAME  "infotecs_module"
#define PATH_TO_SYS        "/sys/kernel"
#define PROC_FILE_NAME_VAR "proc_file_name"
#define TIMER_SECONDS_VAR  "timer_seconds"
#define PROCFS_FN_SIZE     32

#define ERROR              1

#define LAST_ELEMENT(array_name, size) array_name[size - 1]

void print_usage();

void print_invalid_args();

// Reads proc_filename interactively
// Parameters:
//     output_filename - char pointer to the buffer, where
//                       the read filename will be put
// Return value:
//     1 on failure, 0 on success
int read_output_filename_inter(char *output_filename);

// Reads timer_seconds interactively
// Parameters:
//     timer_seconds - pointer to the timer_seconds variable, 
//                     where the read result will be put.
// Return value:
//     1 on failure, 0 on success   
int read_timer_seconds_inter(int *timer_seconds);

// I have to reimplement a safe verision of gets, since stdio.h
// doesn't define it and "gets" is super-vulnerable to buffer
// overflow attacks.
// gets_s() performs bounds-checking, making it more secure.
// 
// Parameters: 
//     char *buf    - pointer to an array of chars
//     size_t count - amount of chars to be read 
// Return value:
//     size_t - amount of bytes read from stdin
// Notes:
//     If the read string size >= count, then null-terminator is
//     appended at the buf[count - 1] position to avoid overflows.
size_t gets_s(char *buf, size_t count);


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

	char output_filename[PROCFS_FN_SIZE] = "hellok";

	if (argc < 2) {

		// read the filename interactively and check the result 
		if (read_output_filename_inter(output_filename) == ERROR
	     || !check_str_valid(output_filename)) {

			printf("Filename not valid. Extinig...\n");
			return ERROR;
		}

		// read the timer parameter interactively and check the result 
		if (read_timer_seconds_inter(&timer_seconds) == ERROR) {
			printf("Invalid integer. Exiting...\n");
			return ERROR;
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
			return ERROR;
		}

		// output_filename is always penultimate.
		// if the name is too long, it will get truncated.
		strncpy(output_filename, argv[argc - 2], PROCFS_FN_SIZE - 1);

		// then read an int from argv 
		int read = sscanf(argv[argc - 1], "%d", &timer_seconds);

		if (read == 0 || read == EOF) {
			printf("Invalid integer. Exiting...\n");	
			return ERROR;
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
		return 1;
	}

	fprintf(proc_file_name_fstream, "%s", output_filename);
	fprintf(timer_seconds_fstream, "%d", timer_seconds);

	// close streams in the end
	fclose(proc_file_name_fstream);
	fclose(timer_seconds_fstream);

	return 0;
}

int read_output_filename_inter(char *output_filename) 
{
	printf("Output lines to (path must not contain whitespaces): /proc/");
	
	// read with gets_s
	size_t read = gets_s(output_filename, PROCFS_FN_SIZE);

	// if nothing was read - it's a failure
	return read == 0;
}

int read_timer_seconds_inter(int *timer_seconds) 
{
	printf("Set timer to: ");
	int read_status = scanf("%d", timer_seconds);

	// if no integer was found or it nothing was given on input - 
	// it's a failure.
	return read_status == EOF || read_status == 0;
}

size_t gets_s(char *buf, size_t count) 
{
	char c;
	size_t strsize = 0;
	
	for (size_t i = 0; i < count; i++) {
		// read char from stdin
		c = getchar();

		// if nothing in stdin or stop-chars - break the loop
		if (c == EOF || c == '\n' || c == ' ') {
			break;
		}
		
		// put char into buffer
		buf[i] = c;

		// increment string size
		strsize++;
	}

	// if string size without terminator is equal to count. then 
	// return a truncated null-terminated string
	if (strsize == count || strsize == 0) {
		LAST_ELEMENT(buf, count) = '\0';
		return strsize;
	}

	// append null-terminator at the end and increment the string size
	buf[strsize++] = '\0';

	return strsize;
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

void print_usage() 
{
	printf("Usage: ./confapp [output_filename] [timer_seconds]\n"
		   "    output_filename - name of the /proc dir file that will\n"
		   "                      contain module output\n"
		   "    timer_seconds   - amount of seconds between consecutive\n"
		   "                      outputs\n"
		   "Or\n"
		   "    Run without arguments for interactive mode.\n\n"
		   "This app writes to the /sys directory, so please,\n"
		   "run it as root.\n"
		);
}

void print_invalid_args() 
{
	printf("Invalid arguments. Type --help to see usage.\n"); 
}