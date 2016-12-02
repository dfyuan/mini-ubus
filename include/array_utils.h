#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <pthread.h>
#include <sys/select.h>
#include <sys/time.h>
#ifndef _ARRAY_UTILS_H
#define _ARRAY_UTILS_H

#define MAX_ARRAY1D_LEN    128

struct array2d_str_s {
	int array_len;
	int valid_len;
	char(*array_ptr)[MAX_ARRAY1D_LEN];
};

extern int array2d_malloc(struct array2d_str_s *array2d_str, int array_len);
extern int array2d_free(struct array2d_str_s *array2d_str);
extern char *get_array2d_str_byIndex(struct array2d_str_s *array2d_str, int index);
extern int set_array2d_str_valid_len(struct array2d_str_s *array2d_str, int len);
extern int get_array2d_str_valid_len(struct array2d_str_s *array2d_str);

#endif