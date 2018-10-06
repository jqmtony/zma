
#include "commands.h"
   
static long openmax = 0;  

static pid_t *childpid = NULL;  
                        /* ptr to array allocated at run-time */  
static int maxfd;  /* from our open_max(), {Prog openmax} */  



static long open_max(void)  
{  
    if (openmax == 0) {      /* first time through */  
        errno = 0;  
        if ((openmax = sysconf(_SC_OPEN_MAX)) < 0) {  
           if (errno == 0)  
               openmax = OPEN_MAX_GUESS;    /* it's indeterminate */  
           else  
               printf("sysconf error for _SC_OPEN_MAX");  
        }  
    }  
   
    return(openmax);  
}  
  

FILE* vpopen(const char *cmdstring, const char *type, int timeout, struct vstat *vt) 
{  
    pid_t pid, pc;
    FILE    *fp;  
    int sval, pfd[2]; 
    long st_time = time(NULL);
  
    /*only allow "r" or "w" */
    if ((type[0] != 'r' && type[0] != 'w') || type[1] != 0) {   
        printf("vpopen() flag error/n");  
        return NULL;  
    }  
    
    if (childpid == NULL) {     /* first time through */  
        /*allocate zeroed out array for child pids */
        maxfd = open_max();  
        if ( (childpid = (pid_t *)calloc(maxfd, sizeof(pid_t))) == NULL)  
            return NULL;  
    }  
    
    if(pipe(pfd)<0)
    {  
        printf("vpopen() pipe create error/n");  
        return NULL;  
    }   
    
    if( (pid = fork()) < 0 )   
        return NULL;  
    else if(0 == pid) {  
        if(*type == 'r')  {  
            close(pfd[0]);    
            if (pfd[1] != STDOUT_FILENO) {  
                dup2(pfd[1], STDOUT_FILENO);  
                close(pfd[1]);  
            }  
        }  
        else{  
            close(pfd[1]);  
            if (pfd[0] != STDIN_FILENO) {  
                dup2(pfd[0], STDIN_FILENO);  
                close(pfd[0]);  
            }  
            
        }  
        /*close all descriptors in childpid[] */
        for (int i = 0; i < maxfd; i++)  
            if (childpid[ i ] > 0)  
                close(i);  
      
        execl(SHELL, "sh", "-c", cmdstring, (char *) 0);  
        _exit(127);  
    }  
    
    if (*type == 'r') {  
        close(pfd[1]);  
        if ( (fp = fdopen(pfd[0], type)) == NULL)  
            return NULL;  
    } else {  
        close(pfd[0]);  
        if ( (fp = fdopen(pfd[1], type)) == NULL)  
            return NULL;  
    }  
    
    childpid[fileno(fp)] = pid; /* remember child pid for this fd */  
    
    do {
        //sem_getvalue(&END_EVENT, &sval);
        //if ( sval !=0 ){
        //    vt->status = PROCEND;
        //    break;
        //}
        if ( (time(NULL) - st_time) >= timeout ) { /*run timeout */
            vt->status = TIMEOUT;
            break;
        }
        
        pc =waitpid(pid, NULL, WNOHANG); 
        if( pc==0 ) {          /*No child exited*/
            usleep(500000);
        } else if (pc == -1) {    /*waitpid error kill */
            vt->status = WAITERROR;
        } else {
            vt->status = NOMALQUIT;
            break;
        }
    }while(pc==0); 
    
    vt->fp = fp;
    vt->pid = pid;
    return fp;  
}  


int vpclose(FILE *fp) 
{  
    int fd;    
    pid_t   pid;    
    
    if (childpid == NULL)    
        return -1;     /* popen() has never been called */    
    
    fd = fileno(fp);    
    if ( (pid = childpid[fd]) == 0)    
        return -1;     /* fp wasn't opened by popen() */    
        
    childpid[fd] = 0;    
    if (fclose(fp) == EOF)    
        return -1;    
        
    return 0;
}
    
void vfree(void) {
    if (childpid != NULL)
        free(childpid);
}
