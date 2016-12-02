#ifndef LOG_TAG_H_
#define LOG_TAG_H_

#define LOG_TAG "BOARD_INFO"

extern int _log_v;

#define INFO(fmt, arg...)   do { if (_log_v >= 5) printf("[5] <%s> <%s:%d> "fmt, LOG_TAG, __FILE__,  __LINE__, ##arg); } while(0)
#define DBG(fmt, arg...)    do { if (_log_v >= 4) printf("[4] <%s> <%s:%d> "fmt,LOG_TAG,  __FILE__,  __LINE__, ##arg); } while(0)
#define MSG(fmt, arg...)    do { if (_log_v >= 3) printf("[3] <%s> <%s:%d> "fmt, LOG_TAG, __FILE__,  __LINE__, ##arg); } while(0)
#define WARN(fmt, arg...)   do { if (_log_v >= 2) fprintf(stderr, "[2] <%s> <%s:%d> "fmt, LOG_TAG, __FILE__,  __LINE__, ##arg); } while(0)
#define ERR(fmt, arg...)    do { if (_log_v >= 1) fprintf(stderr, "[1] <%s> <%s:%d> "fmt, LOG_TAG, __FILE__,  __LINE__, ##arg); } while(0)

#endif
