/*
 * zkClient.h
 */
#ifndef _ZKCLIENT_H_20130813_
#define _ZKCLIENT_H_20130813_

#include <iostream>
#include <vector>
#include <unistd.h>
#include "zookeeper.h"
#include "zookeeper_log.h"

#include <string.h>

#define MAX_PATH_LENGTH 500

typedef void (*watcher_cb)(int type, int state, const char *path,void *watcherCtx);

class ZKClient {
    private:
        zhandle_t *handler;
        static void connection_watcher(zhandle_t *zh, int type, int state, const char *path,void *watcherCtx);
        static void connection_init_watcher(zhandle_t *zh, int type,int state, const char *path,void *watcherCtx);

    public:
        int timeout;
        int connected;
        const char *hosts;
        watcher_cb watcher;
        static ZKClient *m_zk;
        

        int zopen();
        int zclose();
        int reconnect();
        int create(const char* path, const char* value = "", int valuelen = 0, int flags = 0,
                        const struct ACL_vector* acl = &ZOO_OPEN_ACL_UNSAFE);    
        std::vector<std::string> get_children(const char* path, int iswatch = 0);
        int remove(const char* path, int version = -1);
        int exists(const char* cpath, struct Stat* stat = NULL, int iswatch = 0);
        char* get(const char* path, char* value, int* valuelen, struct Stat* stat = NULL, int iswatch = 0);
        int zstat(const char* path, struct Stat* stat);
   
        ZKClient(const char* hosts, int timeout, watcher_cb watcher);
        virtual ~ZKClient();
};

#endif
