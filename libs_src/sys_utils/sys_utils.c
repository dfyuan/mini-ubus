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

#include "logd.h"
#include "sys_utils.h"

/* millisecond precision */
inline long get_sys_uptime()
{
	long uptime_ms;
	struct timespec ts;

	clock_gettime(CLOCK_MONOTONIC, &ts);
	uptime_ms = ts.tv_sec * 1000 + ts.tv_nsec / (1000 * 1000);

	return uptime_ms;
}

void idle_func(void * argc)
{
	MSG("idle func - nothing to do\n");
}

int os_pthread_create(pthread_t *thread, void *(*start_routine)(void *), int thread_attr_size)
{
	int ret;
	pthread_attr_t thread_attr;

	pthread_attr_init(&thread_attr);
	pthread_attr_setstacksize(&thread_attr, thread_attr_size);
	ret = pthread_create(thread, &thread_attr, start_routine, 0);

	return ret;
}

int os_pthread_join(pthread_t thread, void **retval)
{
	int ret = pthread_join(thread, retval);
	return ret;
}

/*
 * del spare character,
 * eg: "787 root       0:00 /bin/busybox udhcpc eth0" ->
 *  "787 root 0:00 /bin/busybox udhcpc eth0"
 */
int del_spare_character(char *buf_in, int len_in, char *buf_out, int character)
{
	int i = 0;
	int j = 0;
	int last_char;

	for (i = 0; i < len_in; i++) {
		if (buf_in[i] == character && last_char == character) {
			continue;
		} else {
			buf_out[j++] = buf_in[i];
			last_char = buf_in[i];
		}
	}
	buf_out[j] = '\0';

	return j;
}

int del_all_character(char *buf_in, int len_in, char *buf_out, int character)
{
	int i = 0;
	int j = 0;

	for (i = 0; i < len_in; i++) {
		if (buf_in[i] == character) {
			continue;
		} else {
			buf_out[j++] = buf_in[i];
		}
	}
	buf_out[j] = '\0';

	return j;
}

/* old one is something wrong */
int os_strsep(char *str, struct string_array *str_array, const char *delim)
{

	char* string = str;
	int num = 0;
	char *key_point;
	while (key_point = strsep(&string, delim)) {
		if (*key_point == 0)
			continue;
		//printf("%s\n", key_point);
		strcpy(str_array->string[num++], key_point);
		if (num >= PACKET_NUM) {
			num = PACKET_NUM - 1;
			break;
		}
	}

	str_array->string_len = num;

	return num;
}

int system_with_res(char* cmdstring, char* buf, int len)
{
	int   fd[2];
	pid_t pid;
	int   n, count;

	memset(buf, 0, len);
	if (pipe(fd) < 0)
		return -1;
	if ((pid = fork()) < 0)
		return -1;
	else if (pid > 0) {
		close(fd[1]);
		count = 0;
		while ((n = read(fd[0], buf + count, len)) > 0 && count > len)
			count += n;
		//printf("buf= %s\n", buf);
		close(fd[0]);
		if (waitpid(pid, NULL, 0) > 0)
			return -1;
	} else {
		close(fd[0]);
		if (fd[1] != STDOUT_FILENO) {
			if (dup2(fd[1], STDOUT_FILENO) != STDOUT_FILENO) {
				return -1;
			}
			close(fd[1]);
		}
		if (execl("/bin/sh", "sh", "-c", cmdstring, (char*)0) == -1)
			return -1;
	}
	return 0;
}

int get_pid_by_cmdline(char *cmd_line)
{
	int ret = -1;
	int	pid;
	DIR	*dir;
	struct dirent	*d;
	char path[128];
	char cmdline[128];
	char cmd_line_no_space[128];

	del_all_character(cmd_line, strlen(cmd_line), cmd_line_no_space, ' ');
	//printf("cmd_line_no_space = %s\n", cmd_line_no_space);

	dir = opendir("/proc");
	if (!dir) {
		printf("open dir(%s) error\n", "/proc");
		return -1;
	}

	while ((d = readdir(dir)) != NULL) {
		/* check if this is a process */
		if ((pid = atoi(d->d_name)) == 0)
			continue;

		snprintf(path, sizeof(path), "/proc/%s/cmdline", d->d_name);
		read_file(path, cmdline, 128);
		if (!strncmp(cmd_line_no_space, cmdline, strlen(cmd_line_no_space))) {
			printf("find process (%s: %d)\n", cmdline, pid);
			ret = pid;
			break;
		}
	}

	closedir(dir);

	return  ret;
}

int run_command(const char *cmd)
{
	int ret = system(cmd);
	if (ret != 0) {
		DBG("# run \"%s\" failed, ret = %d\n", cmd, ret);
	}

	return ret;
}

void dump_string_array(struct string_array str_array)
{
#if 0
	printf("**************************dump string array************************\n");
	int i = 0;
	for (i = 0; i < str_array.string_len; i++) {
		printf("string[%d] = %s\n", i, str_array.string[i]);
	}
	printf("*******************************************************************\n");
#endif
}

#if 0
/* util func for save */
int get_pid_by_process_name(char *process_name)
{
	int i;
	char buf[128];
	char buf_out[128];
	char buf_res[12][12];
	int pid;

	system_with_res("ps | grep '/bin/busybox'", buf, 128);
	printf("11 buf = %s\n", buf);

	del_spare_character(buf, strlen(buf), buf_out, ' ');
	printf("buf - %s\n", buf_out);
	int num = os_strsep(buf_out, buf_res, " ");

	printf("**************************************\n");
	for (i = 0; i < num; i++) {
		printf("%s\n", buf_res[i]);
	}
	printf("**************************************\n");


	printf("buf_res[1] = %s\n", buf_res[1]);
	pid = atoi(buf_res[1]);

	printf("pid = %d\n", pid);
}
#endif
#if 0
int os_strsep(char *str, struct string_array str_array, char *delim)
{
	int num = 0;
	char *p;
	char *buff;
	int i;

	buff = str;
	MSG("buff = %s\n", buff);
	p = strsep(&buff, delim);
	while (p) {
		if (strcmp(p, delim)) {
			num++;
			strcpy(str_array.string[num], p);
			printf("str_array.string[num]: %s\n", str_array.string[num]);
		}
		p = strsep(NULL, delim);
	}

	printf("\n");
#if 0
	printf("num = %d\n", num);
	for (i = 0; i < num; i++) {
		printf("111 %s\n", str_res[i]);
	}
#endif
	str_array.string_len = num;
	return num;
}
#endif
