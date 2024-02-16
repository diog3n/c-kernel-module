#pragma once
#include <stdlib.h>

// Reads procfs_filename interactively
// Parameters:
//     procfs_filename - char pointer to the buffer, where
//                       the read filename will be put
// Return value:
//     1 on failure, 0 on success
int read_procfs_filename_inter(char *procfs_filename);

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
//     amount of bytes read from stdin
// Notes:
//     If the read string size >= count, then null-terminator is
//     appended at buf[count - 1] to avoid overflows. The resulting 
//     string will be truncated.
size_t gets_s(char *buf, size_t count);