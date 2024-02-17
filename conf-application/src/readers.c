#include <stdio.h>

#include "defs.h"
#include "readers.h"

read_status_t read_procfs_filename_inter(char *procfs_filename) 
{
	printf("Output lines to (path must not contain whitespaces): /proc/");
	
	// read with gets_s
	return gets_s(procfs_filename, PROCFS_FN_SIZE + 1);
}

read_status_t read_timer_seconds_inter(int *timer_seconds) 
{
	printf("Set timer to: ");
	int read_status = scanf("%d", timer_seconds);

	// if no integer was found or it nothing was given on input - 
	// it's a failure.
	if (read_status == EOF || read_status == 0) 
		return READ_NONE;

	return READ_OK;
}

read_status_t gets_s(char *buf, size_t count) 
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

	if (strsize == 0) return READ_NONE;

	// if string size without terminator is equal to count, then return
	// invalid non-null-terminated string 
	if (strsize == count) return READ_OVERFLOW;

	// if string is okay then, append null-terminator at the end and 
	// increment the string size
	buf[strsize++] = '\0';
	return READ_OK;
}