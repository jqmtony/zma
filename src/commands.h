#ifndef _COMMANDS_H
#define _COMMANDS_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>  
#include <errno.h>  
#include <fcntl.h>  
#include <semaphore.h>
#include <sys/types.h>
#include <signal.h>


#define SHELL   "/bin/sh"  
   
/* 
 * If OPEN_MAX is indeterminate, we're not 
 * guaranteed that this is adequate. 
 */  
#define OPEN_MAX_GUESS 1024  
#define PROCEND        0
#define NOMALQUIT      1
#define TIMEOUT       -1
#define WAITERROR     -2

struct vstat {
    int pid; 
    int status;   
    FILE *fp; 
};

FILE* vpopen(const char *cmdstring, const char *type, int timeout, struct vstat *vt);
int vpclose(FILE *fp);
void vfree(void);

#endif
