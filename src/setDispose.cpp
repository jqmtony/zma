#include "setDispose.h"

SetDispose::SetDispose() {
    disposeMap[std::string(MONITOR_CRON)] =  opsMonitor;
}

SetDispose::~SetDispose() {
    disposeMap.clear();
}

disposeFunc SetDispose::getDispose(std::string type) {
    return disposeMap[std::string(type)];
}

int SetDispose::opsMonitor(cJSON *detail, void *handle, configuration *gconfig) {
    int ret;
    if (!detail) {
        fprintf(stderr, "Error detail\n");
        return -1;
    } 
    cJSON *name =  cJSON_GetObjectItem(detail, "name");
    cJSON *status = cJSON_GetObjectItem(detail, "status");
    
    if (status && status->valueint) {
        cJSON *command = cJSON_GetObjectItem(detail, "command");
        cJSON *fcontent = cJSON_GetObjectItem(detail, "fcontent");
        cJSON *interval = cJSON_GetObjectItem(detail, "interval");
        cJSON *filename = cJSON_GetObjectItem(detail, "filename");
        if (fcontent && fcontent->valuestring) {
            FILE *f; 
            Base64 base64 = Base64();
            int len = strlen(fcontent->valuestring);
            std::string realcontent = base64.decode(fcontent->valuestring, len );
            std::string filePath = std::string(gconfig->cronsavepath) + std::string("/") + std::string(filename->valuestring);
            
            struct stat fileStat;
            int pos = filePath.find_last_of('/');
            std::string dir = filePath.substr(0, pos );
            if ((stat(dir.c_str(), &fileStat) != 0) || S_ISDIR(fileStat.st_mode)) 
                mkdir(dir.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH ); //0755
            f=fopen(filePath.c_str(),"w");fwrite(realcontent.c_str(),1, strlen(realcontent.c_str()) , f);fclose(f);
            ret = ((DispatcherTimer *)handle)->add_executor(name->valuestring, interval->valueint, command->valuestring );
        }
    } else if (status && status->valueint == 0 ) {
        ret = ((DispatcherTimer *)handle)->delete_executor(name->valuestring);
    } else {
        //
    }
    printf("do %s %d\n", name->valuestring, ret );
    return ret;
}