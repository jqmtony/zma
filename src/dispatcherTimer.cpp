#include "dispatcherTimer.h"

extern configuration gconfig;

void DispatcherTimer::trigger_executor(union sigval v) {
    FILE * fp;
    struct vstat vt;
    char out[1024] = {0};
    std::string command = std::string("cd ") +
                          std::string(gconfig.cronsavepath) +
                          std::string(";") +
                          std::string((char *)v.sival_ptr);
    fp = vpopen(command.c_str(), "r", 180, &vt);
    if(!fp) {
        printf("%s vopen error",command.c_str());
    } 
    else if ( vt.status == TIMEOUT || vt.status == WAITERROR || vt.status == PROCEND ) {   /*超时或者异常*/
        int rs = kill( vt.pid, SIGTERM);
        printf("%s run timeout or waitpid error, kill process %d,%d", command.c_str(), vt.pid,rs);
    } else {
        fgets(out,1024 ,fp); // 读取一行数据
        printf("command: %s\nout: %s\n",command.c_str(), out );
        vpclose(fp);
    }
}


int DispatcherTimer::add_executor(const char *name, int interval, char *command ) {
    timer_t timerid;
    
    struct itimerspec it;
    it.it_interval.tv_sec = interval;  
    it.it_interval.tv_nsec = 0;  
    it.it_value.tv_sec = 1;  
    it.it_value.tv_nsec = 0;  
    
    if ( 0 != is_activity(name, &it) ) { // no exsits, create
        struct sigevent evp;
        memset(&evp, 0, sizeof(struct sigevent));
        evp.sigev_value.sival_ptr = command;    
        evp.sigev_notify = SIGEV_THREAD; 
        evp.sigev_notify_function = trigger_executor;
        
        if (timer_create(CLOCKID, &evp, &timerid) == -1) {  
            perror("fail to timer_create");  
            return -1;;  
        } 
        executors[std::string(name)] =  timerid;
        //executors.insert( std::pair<std::string, timer_t>( std::string(name), timerid ) );
    } else {
        timerid = executors[std::string(name)];
    }
    return timer_settime(timerid, 0, &it, NULL);
}

int DispatcherTimer::delete_executor(const char *name) {
    std::map<std::string, timer_t>::iterator iter;
    iter = executors.find(std::string(name));
    if( iter==executors.end() ) 
        return -1;
        
    timer_t timerid = iter->second;
    executors.erase(iter);
    return timer_delete(timerid);
}

int DispatcherTimer::is_activity(const char *name, struct itimerspec *add) {
    struct itimerspec curr;
    std::map<std::string, timer_t>::iterator iter;
    iter = executors.find(std::string(name));
    if ( iter==executors.end() || 0 != timer_gettime(iter->second, &curr) ) {
        return -1;
    }
    if (add->it_interval.tv_sec == curr.it_interval.tv_sec 
        && add->it_interval.tv_nsec == curr.it_interval.tv_nsec ) {
        return 0;
    }
    return -2;
}
