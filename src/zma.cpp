/* 
 * @author xuyunj
*/
#include <unistd.h>
#include <signal.h>
#include <semaphore.h>
#include <vector>
#include "common.h"
#include "base64.h"
#include "cJSON.h"
#include "zkClient.h"
#include "dispatcherTimer.h"
#include "setDispose.h"

sem_t END_EVENT;
configuration gconfig;    //全局声明
SetDispose setdo = SetDispose();
DispatcherTimer dt = DispatcherTimer();


// Program termination signal.
void endSignalHandler(int prama) {
    sem_post(&END_EVENT);
}

int do_data_route(const char* path, char *buff) {
    cJSON *root = NULL;
    root = cJSON_Parse(buff);     
    if (!root) {
        printf("Error before: [%s]\n",cJSON_GetErrorPtr());
        return -1;
    } 
    cJSON *type =  cJSON_GetObjectItem(root, "type");
    if (!type) {
        printf("Error type: [%s]\n",cJSON_GetErrorPtr());
        return -1;
    }
    std::string htype(type->valuestring);
    int (*disposeFunc)(cJSON*, void*, configuration*);
    disposeFunc = setdo.getDispose(std::string(type->valuestring));
    if (!disposeFunc)
        return -2;
    return disposeFunc(cJSON_GetObjectItem(root, "detail"), &dt, &gconfig);
}

void load_child_data(ZKClient *zk, std::string rootPath) {
   
    if ( zk->exists( rootPath.c_str() ) != ZOK ) {
        zk->create( rootPath.c_str() );
    }    
    
    std::vector<std::string>::iterator it;
    std::vector<std::string> lchildren = zk->get_children( rootPath.c_str(), 1);
    for ( it = lchildren.begin(); it != lchildren.end(); ++it ) {
        struct Stat st;
        std::string childPath = rootPath + std::string("/") + (*it);
        zk->zstat(childPath.c_str(), &st); 
        int dataLength = st.dataLength + 1;
        char *buff=(char*)malloc( dataLength );
        zk->get(childPath.c_str(), buff, &dataLength, NULL, 1);
        do_data_route(childPath.c_str(), buff );
        free(buff);
    }
}

void do_watcher_event(int type, int state, const char* path, void* watcherCtx ) {
    if (type == ZOO_CHILD_EVENT) {
        printf("child path: %s\n", path );
        load_child_data(ZKClient::m_zk, std::string(path)); 
    }
    if (type == ZOO_DELETED_EVENT) {
        printf("deleted path: %s\n", path );
        std::string dpath(path);
        int pos = dpath.find('/', 1);
        std::string name = dpath.substr(pos+1, -1);
        dt.delete_executor(name.c_str());
    }
    if (type == ZOO_CHANGED_EVENT) {
        struct Stat st;
        ZKClient::m_zk->zstat(path, &st);
        int dataLength = st.dataLength + 1;
        char *buff=(char*)malloc( dataLength );
        ZKClient::m_zk->get(path, buff, &dataLength, NULL, 1);
        do_data_route(path, buff);
        printf("changed path[%s]: %s\n", path, buff );
        free(buff);
    } 
}

int main(int argc, char *argv[] ) {
    const char *inifile = argv[1];
    if ( 0 != load_config(inifile, &gconfig)) {
        fprintf(stderr, "Can't load 'test.ini'\n");
        return 1;
    }
    std::string rootPath(gconfig.rootpath);
    sem_init(&END_EVENT, 0, 0);
    signal(SIGTERM,endSignalHandler); 
    ZKClient zk = ZKClient(gconfig.zoohosts, 3000, do_watcher_event);
    zk.zopen();
    load_child_data(&zk, rootPath);
    sem_wait(&END_EVENT);
    return 0;
} 
