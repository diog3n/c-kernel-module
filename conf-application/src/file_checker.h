#pragma once

typedef enum file_check_result {
	FILE_EXISTS,
	FILE_DOES_NOT_EXIST
} f_check_res_t;

// checks if /proc/<proc_file_name> exists
// Parameters:
//     const char *proc_file_name - name of the file to be checked
// Return value:
//     FILE_EXISTS or FILE_DOES_NOT_EXIST respectively
f_check_res_t check_proc_file_exists(const char *proc_file_name);