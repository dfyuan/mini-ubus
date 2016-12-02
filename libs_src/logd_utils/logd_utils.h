#ifndef _LOGD_H
#define _LOGD_H

int logd_init(char *log_file_path);
int logd_out(char *logd_buf, int logd_len);
int logd_deinit();

#endif
