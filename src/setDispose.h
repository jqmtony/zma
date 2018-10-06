#ifndef SETDISPOSE_H
#define SETDISPOSE_H
#include <map>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "common.h"
#include "cJSON.h"
#include "base64.h"
#include "dispatcherTimer.h"


#define MONITOR_CRON "cron"

typedef int (*disposeFunc)(cJSON*, void*, configuration*);

class SetDispose {

    public:
        int a;
        std::map<std::string, disposeFunc> disposeMap;
        
        disposeFunc getDispose(std::string type);
        static int opsMonitor(cJSON *detail, void *handle, configuration *gconfig = NULL);
        SetDispose();
        virtual ~SetDispose();
};


#endif