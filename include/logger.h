#ifndef __LOGGER_H__
#define __LOGGER_H__

#define LOG_LEVEL_ERROR    7
#define LOG_LEVEL_WARN     6
#define LOG_LEVEL_INFO     5
#define LOG_LEVEL_DEBUG    4

#define loge(fmt, args...) \
    debug(LOG_LEVEL_ERROR, "%s %d : " fmt,  __FUNCTION__, __LINE__, ##args)

#define logw(fmt, args...) \
    debug(LOG_LEVEL_WARN,  "%s %d : " fmt,  __FUNCTION__, __LINE__, ##args)

#define logi(fmt, args...) \
    debug(LOG_LEVEL_INFO,  "%s %d : " fmt,  __FUNCTION__, __LINE__, ##args)

#define logd(fmt, args...) \
    debug(LOG_LEVEL_DEBUG, "%s %d : " fmt,  __FUNCTION__, __LINE__, ##args)


void logger_init( char *path );
void debug( int level, const char *fmt, ... );

#endif

