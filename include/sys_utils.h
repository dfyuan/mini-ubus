#ifndef _UTILS_H
#define _UTILS_H
#include "file_utils.h"
#include "array_utils.h"

#define PACKET_NUM     12
struct string_array {
	char string[PACKET_NUM][64];
	int string_len;
};

extern inline long get_sys_uptime();
extern void idle_func(void *);
extern int os_pthread_create(pthread_t *thread, void *(*start_routine)(void *), int thread_attr_size);
extern int os_pthread_join(pthread_t thread, void **retval);

extern int del_spare_character(char *buf_in, int len_in, char *buf_out, int character);
extern int del_all_character(char *buf_in, int len_in, char *buf_out, int character);
extern int os_strsep(char *str, struct string_array *str_array, const char *delim);
extern int system_with_res(char* cmdstring, char* buf, int len);
extern int get_pid_by_cmdline(char *cmd_line);
extern void dump_string_array(struct string_array str_array);
extern int run_command(const char *cmd);

#endif
