#include <stdio.h>

#include "printers.h"

void print_usage() 
{
	printf("Usage: ./confapp [procfs_filename] [timer_seconds]\n"
		   "    procfs_filename - name of the /proc dir file that will\n"
		   "                      contain module output\n"
		   "    timer_seconds   - amount of seconds between consecutive\n"
		   "                      outputs\n"
		   "Or\n"
		   "    Run without arguments for interactive mode.\n\n"
		   "This app writes to the /sys directory, so please,\n"
		   "run it as root.\n"
		);
}

void print_invalid_filename() 
{
	printf("Filename not valid. Extinig...\n");	
}

void print_invalid_timer_seconds() 
{
	printf("Invalid timer expiration value. Exiting...\n");	
}

void print_invalid_args() 
{
	printf("Invalid arguments. Type --help to see usage.\n"); 
}