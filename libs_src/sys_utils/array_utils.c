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
#include <sys/socket.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <errno.h>
#include <sys/sysinfo.h>
#include <dirent.h>
#include <sys/wait.h>
#include <dirent.h>

#include "logd.h"
#include "array_utils.h"

int array2d_malloc(struct array2d_str_s *array2d_str, int array_len)
{
	array2d_str->array_ptr = (char(*)[MAX_ARRAY1D_LEN])malloc(sizeof(char) * array_len * MAX_ARRAY1D_LEN);
	array2d_str->array_len = array_len;
	return 0;
}

int array2d_free(struct array2d_str_s *array2d_str)
{
	free(array2d_str->array_ptr);
	return 0;
}

char *get_array2d_str_byIndex(struct array2d_str_s *array2d_str, int index)
{
	if (index > array2d_str->array_len) {
		printf("index(%d) out of range\n", index);
		return NULL;
	}

	return array2d_str->array_ptr[index];
}

int set_array2d_str_valid_len(struct array2d_str_s *array2d_str, int len)
{
	array2d_str->valid_len = len;
	return 0;
}

int get_array2d_str_valid_len(struct array2d_str_s *array2d_str)
{
	return array2d_str->valid_len;
}

