#include "zkClient.h"
#include <string>
#include <errno.h>

ZKClient* ZKClient::m_zk;

ZKClient::ZKClient(const char* hosts, int timeout, watcher_cb watcher) {
    m_zk = this;
    this->hosts = hosts;
    this->timeout = timeout;
    this->watcher = watcher;
}
    
ZKClient::~ZKClient() { zclose(); }

void ZKClient::connection_init_watcher(zhandle_t *zh, int type, 
        int state, const char *path,void *watcherCtx) {
        m_zk->handler = zh;
        m_zk->connected = 1;
        
        if (NULL != m_zk->watcher )
            zoo_set_watcher(zh, connection_watcher);
}

void ZKClient::connection_watcher(zhandle_t *zh, int type, 
        int state, const char *path,void *watcherCtx) {
        //void (*func)(int, int, const char*, void*);
        m_zk->watcher(type, state, path, watcherCtx);
        zoo_set_watcher( zh, connection_watcher );
}

int ZKClient::zopen() {
    handler = zookeeper_init(hosts, connection_init_watcher, timeout, 0, NULL, 0);
    sleep(timeout/1000);
    return connected;
    if (!connected)
        fprintf(stderr, "Unable to connect to %s\n", hosts);
    return connected;
}

int ZKClient::zclose() {
    return zookeeper_close(handler);
}

int ZKClient::reconnect() {
    if (ZOO_CONNECTED_STATE != zoo_state(handler) )
        zclose(),zopen();
    return zoo_state(handler);
}

int ZKClient::create(const char* cpath, const char* value, int valuelen, int flags, const struct ACL_vector* acl) {
    std::string path(cpath);
    if ( exists(path.c_str()) == ZOK ) {
        return ZOK;
    }

    std::string::size_type pos = 0, index = 0;
    do {
        pos = path.find('/', index);
        if (pos == index) {
            index++;
            continue;
        } else {
            index = pos;
        }
        std::string node = path.substr(0, pos);
        if ( exists( node.c_str() ) != ZOK ) {
            int node_type = (pos == -1 ) ? flags : 0;
            int zerr = create( node.c_str(), value, valuelen, node_type, acl);
            if (zerr != ZOK ) {
                return zerr;
            }
        }
    } while ( pos != std::string::npos);
    return ZOK;
}

int ZKClient::remove(const char* path, int version) {
    return zoo_delete(handler, path, version);
}

int ZKClient::exists(const char* path, struct Stat* stat, int iswatch) {
    return zoo_exists(handler, path, iswatch, stat);
}

int ZKClient::zstat(const char* path, struct Stat* stat) {
    return zoo_exists(handler, path, 0, stat);
}

char* ZKClient::get(const char* path, char* buff, int* bufflen, struct Stat* stat, int iswatch) {
    int zerr = zoo_get(handler, path, iswatch, buff, bufflen, stat);
    if (zerr != ZOK) {
        printf("zoo_get: %s", zerror(zerr));
        return NULL;
    }
    return buff;
}

std::vector<std::string> 
ZKClient::get_children(const char* path, int iswatch) {
    std::vector<std::string> lchildren;
    struct String_vector children;
    int zerr = zoo_get_children(handler, path, iswatch, &children);
    if (zerr != ZOK) {
        printf("zoo_get_children: %s", zerror(zerr));
    } else {
        for (int i = 0; i < children.count; ++i) {
            lchildren.push_back(std::string(children.data[i]));
        }
    }
    return lchildren;
}
