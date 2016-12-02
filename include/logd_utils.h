#ifndef _LOGD_UTILS_H
#define _LOGD_UTILS_H
#include <stdio.h>
#include <sys/time.h>

int logd_init(char *log_file_path);
int logd_out(char *logd_buf, int logd_len);
int logd_deinit();

extern char logd_buf[512];

static void sys_time_out()
{
	struct tm *p;
	struct timeval tv;
	struct timezone tz;

	gettimeofday(&tv, &tz);
	p = localtime(&tv.tv_sec);
	printf("%d-%d-%d %d:%d:%d.%ld ", 1900+p->tm_year, 1+p->tm_mon, p->tm_mday, p->tm_hour, p->tm_min, p->tm_sec, tv.tv_usec);
}

#define PRINT(fmt, arg...) \
		do {  \
			if (_log_v >= 6) {   \
				printf(fmt, ##arg); \
				sprintf(logd_buf, fmt, ##arg); \
				logd_out(logd_buf, strlen(logd_buf)); \
			} \
		} while(0)

#define INFO(fmt, arg...) \
		do {  \
			if (_log_v >= 5) {   \
				sys_time_out(); \
				printf("[5] <%s> <%s:%d> "fmt, LOG_TAG, __FILE__,  __LINE__, ##arg); \
			} \
			sprintf(logd_buf, "[5] <%s> <%s:%d> "fmt, LOG_TAG, __FILE__,  __LINE__, ##arg); \
			logd_out(logd_buf, strlen(logd_buf)); \
		} while(0)

#define DBG(fmt, arg...) \
		do {  \
			if (_log_v >= 4) {   \
				sys_time_out(); \
				printf("[4] <%s> <%s:%d> "fmt, LOG_TAG, __FILE__,  __LINE__, ##arg); \
			} \
			sprintf(logd_buf, "[4] <%s> <%s:%d> "fmt, LOG_TAG, __FILE__,  __LINE__, ##arg); \
			logd_out(logd_buf, strlen(logd_buf)); \
		} while(0)

#define MSG(fmt, arg...) \
		do {  \
			if (_log_v >= 3) {   \
				sys_time_out(); \
				printf("[3] <%s> <%s:%d> "fmt, LOG_TAG, __FILE__,  __LINE__, ##arg); \
			} \
			sprintf(logd_buf, "[3] <%s> <%s:%d> "fmt, LOG_TAG, __FILE__,  __LINE__, ##arg); \
			logd_out(logd_buf, strlen(logd_buf)); \
		} while(0)

#define WARN(fmt, arg...) \
		do {  \
			if (_log_v >= 2) {   \
				sys_time_out(); \
				printf("[2] <%s> <%s:%d> "fmt, LOG_TAG, __FILE__,  __LINE__, ##arg); \
			} \
			sprintf(logd_buf, "[2] <%s> <%s:%d> "fmt, LOG_TAG, __FILE__,  __LINE__, ##arg); \
			logd_out(logd_buf, strlen(logd_buf)); \
		} while(0)

#define ERR(fmt, arg...) \
		do {  \
			if (_log_v >= 1) {   \
				sys_time_out(); \
				printf("[1] <%s> <%s:%d> "fmt, LOG_TAG, __FILE__,  __LINE__, ##arg); \
			} \
			sprintf(logd_buf, "[1] <%s> <%s:%d> "fmt, LOG_TAG, __FILE__,  __LINE__, ##arg); \
			logd_out(logd_buf, strlen(logd_buf)); \
		} while(0)

#define BUILD_INFO_LOG() \
	do { \
		char g_debug_info[128]; \
		snprintf(g_debug_info, sizeof(g_debug_info), "%s %s", __DATE__, __TIME__); \
		MSG("build info: < %s > - <%s>\n", git_version, g_debug_info); \
	} while(0)

#endif
