#ifndef _FILES_UTILS_H
#define _FILES_UTILS_H
#include "array_utils.h"

#define MAX_FILE_PATH_LEN     MAX_ARRAY1D_LEN

extern int check_file_exist(char *path);
extern int read_file(char *file_name, char *buf, int len);
extern int get_files_count(char *dir_path);
extern int get_files_list(char *dir_path, struct array2d_str_s *string_array2d);
extern int rename_file(char *old_file_path, char *new_file_path);
extern int rm_file(char *file_path);
extern int cp_file(char *old_file_path, char *new_file_path);

#endif
