#include <stdio.h>

#include "defs.h"
#include "readers.h"

int read_procfs_filename_inter(char *procfs_filename) 
{
	printf("Output lines to (path must not contain whitespaces): /proc/");
	
	// read with gets_s
	size_t read = gets_s(procfs_filename, PROCFS_FN_SIZE);

	// if nothing was read - it's a failure
	return read == 0;
}

int read_timer_seconds_inter(int *timer_seconds) 
{
	printf("Set timer to: ");
	int read_status = scanf("%d", timer_seconds);

	// if no integer was found or it nothing was given on input - 
	// it's a failure.
	return read_status == EOF || read_status == 0 || timer_seconds <= 0;
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