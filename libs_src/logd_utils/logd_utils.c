#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
static int logd_fd;
static int logd_count = 0;
char logd_buf[512] = {0,};

int logd_init(char *log_file_path)
{
	printf("log file path = %s\n", log_file_path);
	logd_fd = open(log_file_path, O_RDWR | O_CREAT);
	if (logd_fd < 0) {
		printf("open log file error (%d)\n", errno);
		return -1;
	}

	return 0;
}

int logd_out(char *logd_buf, int logd_len)
{
	int ret = 0;

	if (logd_fd < 0) {
		printf("logd not inited\n");
		return -1;
	}
	if (logd_count++ %100 == 0) {
		ftruncate(logd_fd, 0);
		lseek(logd_fd, 0, SEEK_SET);
	}

	ret = write(logd_fd, logd_buf, logd_len);
	if (ret != logd_len) {
		printf("logd out error\n");
		return -1;
	}

	return 0;
}

int logd_deinit()
{
	if (logd_fd < 0) {
		printf("logd not inited, no need deinit\n");
		return -1;
	}
	close(logd_fd);

	return 0;
}
