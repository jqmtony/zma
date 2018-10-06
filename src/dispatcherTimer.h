
#ifndef DISPATCHERTIMER_H
#define DISPATCHERTIMER_H

#include <stdio.h>
#include <time.h>
#include <signal.h>
#include <string.h>
#include <map>
#include "common.h"
#include "base64.h"
#include "commands.h"

#define CLOCKID CLOCK_REALTIME

class DispatcherTimer{

    private:
        std::map<std::string, timer_t> executors;
        static void trigger_executor(union sigval v);
    
    public:
        int add_executor(const char *name, int interval, char *command );
        int delete_executor(const char *name);
        int is_activity(const char *name, struct itimerspec *add);
};
#endif
