#pragma once
#include <stdlib.h>

typedef enum read_status {
	READ_OK,
	READ_NONE,
	READ_OVERFLOW
} read_status_t;

// Reads procfs_filename interactively
// Parameters:
//     procfs_filename - char pointer to the buffer, where
//                       the read filename will be put
// Return value:
//     read_status_t of the read operation.
read_status_t read_procfs_filename_inter(char *procfs_filename);

// Reads timer_seconds interactively
// Parameters:
//     timer_seconds - pointer to the timer_seconds variable, 
//                     where the read result will be put.
// Return value:
//     ERROR if nothing read, SUCCESS if successfully read an integer   
read_status_t read_timer_seconds_inter(int *timer_seconds);

// I have to reimplement a safe verision of gets, since stdio.h
// doesn't define it and "gets" is super-vulnerable to buffer
// overflow attacks.
// gets_s() performs bounds-checking, making it more secure.
// 
// Parameters: 
//     char *buf    - pointer to an array of chars
//     size_t count - amount of chars to be read 
// Return value:
//      GETS_S_OK if read less chars than count
//      GETS_S_NONE if read 0 chars
//      GETS_S_OVRFL if read => count.
// Notes:
//     If the read string size >= count, then null-terminator is
//     not appended at the end and iterating over the string will
//     result in buffer overflow. 
read_status_t gets_s(char *buf, size_t count);