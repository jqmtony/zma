#ifndef COMMON_H
#define COMMON_H
#include <string.h>
#include "ini.h"


//define LOGGING_DEBUG dzlog_debug   
//define LOGGING_INFO  dzlog_info
//define LOGGING_ERROR dzlog_error
//define LOG_HEX(buf, buf_len) hdzlog_debug(buf, buf_len)

typedef struct
{
    const char* rootpath;
    const char* zoohosts;
    const char* cronsavepath;
    
    const char* logrule;
    const char* logconf;
} configuration;

int load_config(const char *inifile, configuration *config);


#endif