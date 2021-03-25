#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <stdbool.h>
#include <signal.h>
#include <time.h>

int max=1000;

void proc_exit(int sig){
    FILE *f;
    f = fopen("logger.log", "a+");  // open the file
    if (f == NULL)
        printf(" cannot open the file");
    timer_t t;                    // to print date and time in the log file
    time(&t);
    fprintf(f,"%s  Child process was terminated\n",ctime(&t));
    fclose(f);
}

void printDir() {   //  to print current directory
    char cwd[1024];
    getcwd(cwd, sizeof(cwd));
    printf("Dir: %s\n", cwd);
}

char* substr(const char *src, int m, int n) {
    int len=n-m;
    char *dest= (char*)malloc(sizeof(char) * (len + 1));
    dest[len]='\0';
    strncpy(dest,(src+m),len);
    return dest;
}

// to split using space and return true if its background process and false otherwise
bool getSubStrings(char *ip,char** param){
     int dx=0;               // clear the parameter array
     while(param[dx]!=NULL){
        param[dx]=NULL;
        dx++;
    }
    bool result=false;
    int f=0;                 // first index
    int s=0;                 // second element
    int p=0;                 // index of parameter array
    while(s !=strlen(ip)){
        if(ip[s]==' '){
            param[p]=substr(ip,f,s);
            p++;             // element is added to the parameter array
            f=s+1;           // change the first index-> to the index of space +1
        }
        s++;
     }
     if(ip[s-1]=='&'){
        printf(" BACKGROUND \n");
         result=true;
     }
     else{                  // save the last part of command line as the last parameter
       param[p]=substr(ip,f,s);
       p++;
     }
     return result;
}

int main(){
    while(1){
        printDir();
        printf(">>>");
        char *ip[max];
        char **param[max];
        gets(ip);
        signal(SIGCHLD,proc_exit);
        bool background=getSubStrings(ip,param);
        if(strcmp(ip,"exit")==0)  // exit command
            exit(0);
        else{
            pid_t pid;
            pid=fork();
            //printf(" pid = %d\n",pid);
            if(pid<0)    // error
                printf("fork failed");
            else if(pid==0 ){   // child process
                    if(execvp(param[0],param)<0)    //if the return value less than zero then cannot be executed
                        printf(" wrong command \n");
                    exit(0);
            }
            else {// parent process
                if(!background){
                    wait(NULL);
                }
            }
        }
        printf("*--*--*--*--*--*--*--*--*--*--*--*--*--*--*--*--*--*--*--*--*--*--*--*--*--*\n");
    }
    return 0;
}
