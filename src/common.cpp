#include "common.h"

static int handler(void* agent, const char* section, const char* name,
                   const char* value)
{
    configuration* pconfig = (configuration*)agent;

    #define MATCH(s, n) strcmp(section, s) == 0 && strcmp(name, n) == 0
    if (MATCH("agent", "rootpath")) {
        pconfig->rootpath = strdup(value);
    } else if (MATCH("agent", "zoohosts")) {
        pconfig->zoohosts = strdup(value);
    } else if (MATCH("agent", "cronsavepath")) {
        pconfig->cronsavepath = strdup(value);
    } else if (MATCH("agent", "logconf")) {
        pconfig->logconf = strdup(value);
    }else if (MATCH("agent", "logrule")) {
        pconfig->logrule = strdup(value);
    }else {
        return 0;  /* unknown section/name, error */
    }
    return 1;
}

int load_config(const char *inifile, configuration *config)
{
    if (ini_parse(inifile, handler, config) < 0) {
        //printf("Can't load 'test.ini'\n");
        return 1;
    }
    printf("Config loaded from '%s'\n", inifile );
    return 0;
}