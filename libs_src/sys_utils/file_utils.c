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
#include "sys_utils.h"

int check_file_exist(char *path)
{
	if (path == NULL)
		return -1;

	struct stat st;
	if (!stat(path, &st)) {
		return 1;
	} else {
		return 0;
	}
}

int read_file(char *file_name, char *buf, int len)
{
	int fd;
	int ret = -1;
	int i = 0;
	int j = 0;
	char tmp_buf[128];

	fd = open(file_name, O_RDONLY);
	if (fd < 0) {
		printf("open file (%s) error\n", file_name);
		ret = -1;
		return ret;
	}

	ret = read(fd, tmp_buf, len);
	if (ret > 0) {
		for (i = 0; i < ret; i++) {
			if (tmp_buf[i] != '\0') {
				buf[j++] = tmp_buf[i];
			}
		}

		buf[j] = '\0';
	}

	close(fd);

	return ret;
}

int get_files_count(char *dir_path)
{
	DIR *dir;
	struct dirent *ptr;
	int files_count = 0;

	if ((dir = opendir(dir_path)) == NULL) {
		ERR("open dir(%s) error...\n", dir_path);
		return -1;
	}

	while ((ptr = readdir(dir)) != NULL) {
		if (strcmp(ptr->d_name, ".") == 0 || strcmp(ptr->d_name, "..") == 0)
			continue;
		else if (ptr->d_type == 8) {  ///file_type
			files_count++;
		}
	}

	closedir(dir);

	return files_count;
}

int get_files_list(char *dir_path, struct array2d_str_s *string_array2d)
{
	DIR *dir;
	struct dirent *ptr;
	int file_num = 0;

	if ((dir = opendir(dir_path)) == NULL) {
		ERR("open dir(%s) error...\n", dir_path);
		return -1;
	}

	while ((ptr = readdir(dir)) != NULL) {
		if (strcmp(ptr->d_name, ".") == 0 || strcmp(ptr->d_name, "..") == 0)
			continue;
		else if (ptr->d_type == 8) {  ///file_type
			if (strlen(ptr->d_name) >= 128 - 1) {
				printf("file name (%s) length over 128bytes\n", ptr->d_name);
				return -1;
			}
			//strcpy(string_array2d->array_ptr[file_num], ptr->d_name);
			char *str_ptr = get_array2d_str_byIndex(string_array2d, file_num);
			if (str_ptr != NULL) {
				strcpy(str_ptr, ptr->d_name);
			}
			file_num++;
		}
	}

	set_array2d_str_valid_len(string_array2d, file_num);
	closedir(dir);
	return 0;
}

int rename_file(char *old_file_path, char *new_file_path)
{
	char cmd_buf[1024];

	if (strlen(old_file_path) == 0 && strlen(old_file_path) > MAX_FILE_PATH_LEN) {
		ERR("invalid file name\n");
		return -1;
	}

	if (strlen(new_file_path) == 0 && strlen(new_file_path) > MAX_FILE_PATH_LEN) {
		ERR("invalid file name\n");
		return -1;
	}

	if (check_file_exist(old_file_path) <= 0) {
		ERR("old file(%s) not exist!!!\n", old_file_path);
		return -2;
	}

	if (check_file_exist(new_file_path) > 0) {
		ERR("new file(%s) exist!!!\n", new_file_path);
		return -2;
	}

	sprintf(cmd_buf, "mv %s %s", old_file_path, new_file_path);

	return run_command(cmd_buf);
}

int cp_file(char *old_file_path, char *new_file_path)
{
	char cmd_buf[1024];

	if (strlen(old_file_path) == 0 && strlen(old_file_path) > MAX_FILE_PATH_LEN) {
		ERR("invalid file name\n");
		return -1;
	}

	if (strlen(new_file_path) == 0 && strlen(new_file_path) > MAX_FILE_PATH_LEN) {
		ERR("invalid file name\n");
		return -1;
	}

	sprintf(cmd_buf, "cp %s %s", old_file_path, new_file_path);

	return run_command(cmd_buf);
}

int rm_file(char *file_path)
{
	char cmd_buf[512];

	if (strlen(file_path) == 0 && strlen(file_path) > MAX_FILE_PATH_LEN) {
		ERR("invalid file name\n");
		return -1;
	}

	if (check_file_exist(file_path) <= 0) {
		ERR("file(%s) not exist!!!\n", file_path);
		return -2;
	}

	sprintf(cmd_buf, "rm -f %s", file_path);

	return run_command(cmd_buf);
}
