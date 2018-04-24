#include <stdio.h>   
#include <unistd.h>   
#include <sys/types.h>   
#include <errno.h>  
#include <sys/wait.h>  
#include <stdlib.h>  
#include <string.h>   
   
extern char **getline();   
   
main() {  
  int i;   
  char **args;  
  int pid;  
  int stat_val;  
  int isRedirect;  
  int isPipe;  
  int isBack;  
  int position;   
  
  printf("Welcome to MyShell ^_^\n");   
   
  while(1) {  
      
    isRedirect=0;  
    isPipe=0;  
    position=0;  
   
    printf("MyShell>>");  
    args = getline();  
  
    for(i = 0; args[i] != NULL; i++){    //testing whether the command is redirection or pipe  
      if(!strcmp(args[i],"<")||!strcmp(args[i],">"))  
    {  
      isRedirect=1;  
      position=i;  
    }  
      if(!strcmp(args[i],"|")){  
      isPipe=1;  
      position=i;  
    }  
    }   
      
    if(args[0]==NULL); //empty command  
  
    else if(!strcmp(args[i-1],"&")){//background execution  
      args[i-1] = NULL;  
      pid=fork();//bulid a runback process,and don't eliminate the parent process  
      if(pid==-1) {   
         perror("Failed to fork");  
         return 1;  
      }  
      if(pid == 0){  
         if(setsid() == -1)  
            perror("Child faild to become a session leader");  
         else{   
            execvp(args[0],args);  
            printf("Command not found in PATH\n");  
            exit(0);  
         }  
         return 1;  
      }  
    }  
  
    else if(isPipe){  //pipe  
    pid_t child,gChild;  
    int file_pipes[2];    
    char **command;  
        command=&args[position+1];  
    args[position]=NULL;  
        child=fork();  
        switch(child){  
            case -1:  
              perror("Fork Failed\n");  
              exit(-1);  
  
            case 0:  
              if(!pipe(file_pipes)){  
                  gChild=fork();  
                  switch(gChild){  
                     case -1:  
                        fprintf(stderr,"Fork Failed\n");  
                        exit(-1);  
  
                     case 0:           //the grandchild process,writing to the pipe  
                          
                        close(1);  
                        dup2(file_pipes[1],1);  
                        close(file_pipes[1]);  
                        close(file_pipes[0]);  
                        execvp(args[0],args);  
                        printf("Command not found in PATH\n");  
                        exit(0);      
  
  
  
  
                     default:         //the child process, reading from the pipe  
                          
                        close(0);  
                        dup(file_pipes[0]);  
                        close(file_pipes[0]);  
                        close(file_pipes[1]);  
                        execvp(command[0],command);  
                        printf("Command not found in PATH\n");  
                        exit(0);                       
                    }  
              }    
              default:  
                   waitpid(child,&stat_val,0);  
                   if(!WIFEXITED(stat_val))  
                        printf("Child terminated abnormally\n");  
         }  
                            
    }  
  
    else if(!strcmp(args[0],"exit")) //the exit command  
        exit(0);  
  
      
    else if(!strcmp(args[0],"add")){ //the add command  
        pid=fork();  
          
        switch(pid){  
  
           int i;  
           int result;  
          
           case -1:  
              perror("Fork Failed\n");  
              exit(-1);  
             
           case 0:  
              if(isRedirect){  
                 char *fileName;  
         fileName=args[position+1];  
         args[position]=NULL;  
         if(!freopen(fileName,"w",stdout)){ //Redirect the add result to file  
            fprintf(stderr,"could not redirect stdout to file %s\n",fileName);  
            exit(2);  
             }  
              }      
              result=add(args);  
              for(i=1;args[i+1]!=NULL;i++)  
                 printf("%s+",args[i]);  
              printf("%s=%d\n",args[i],result);  
              exit(0);  
             
           default:  
              wait(&stat_val);  
              if(!WIFEXITED(stat_val))  
                  printf("Error:child terminated abnormally\n");  
    
        }  
   }  
  
   else if(!strcmp(args[0],"args")){ //the args command  
       pid=fork();  
  
       switch(pid){  
  
         case -1:  
            perror("Fork Failed\n");  
            exit(-1);  
  
         case 0:  
            if(isRedirect){  
               char *fileName;  
           fileName=args[position+1];  
           args[position]=NULL;  
         if(!freopen(fileName,"w",stdout)){ //redirect args result to file  
            fprintf(stderr,"could not redirect stdout to file %s\n",fileName);  
            exit(2);  
          }  
            }      
            arg(args);  
            exit(0);  
  
         default:  
            wait(&stat_val);  
            if(!WIFEXITED(stat_val))  
                printf("Error:child terminated abnormally\n");  
      }  
   }  
  
   else if(!strcmp(args[0],"search")){   
        printf("%d found\n",search(args));   
   }   
  
  
  
   else {   //the rest of the cases  
        pid=fork();  
  
        switch(pid){  
  
            case -1:  
              perror("Fork Failed\n");  
              exit(-1);  
  
            case 0:  
              if(isRedirect){  //redirection  
                 char *fileName;  
         fileName=args[position+1];  
         if(*args[position]=='>'){  
            args[position]=NULL;  
            if(!freopen(fileName,"w",stdout)){ //writing to file, closing standard out  
            fprintf(stderr,"could not redirect stdout to file %s\n",fileName);  
            exit(2);  
            }  
            execvp(args[0],args);  
                    fprintf(stderr,"Command not found in PATH\n");  
                    exit(0);  
             }      
             else{  
            args[position]=NULL;  
            if(!freopen(fileName,"r",stdin)){ //reading rom file, closing standard in  
               fprintf(stderr,"could not redirect stdin to file %s\n",fileName);  
               exit(2);  
            }  
            execvp(args[0],args);  
                    printf("Command not found in PATH\n");  
                    exit(0);  
        }  
              }  
              else{          
                   execvp(args[0],args);  
                   printf("Command not found in PATH\n");  
                   exit(0);  
              }  
  
  
            default:  
              wait(&stat_val);  
              if(!WIFEXITED(stat_val))  
                  printf("Error:child terminated abnormally\n");  
       
        }  
     }   
   }  
}  
  
int add(char**args)  
{  
   int sum=0;   
   int i;  
   for(i=1;args[i]!=NULL;i++){  
      if(args[i][0]=='0'&&args[i][1]!='\0'&&(args[i][1]=='x'||args[i][1]=='X'))  
         sum+=hex(args[i]);  
      else  
         sum+=dec(args[i]);  
   }  
   return sum;  
}  
  
int dec(char* str){ //converting string notation of dec number to decimal int  
   int num=0;  
   int i;  
   for(i=0;str[i]!='\0';i++){  
     int digit=str[i]-'0';  
       if(digit<0||digit>9){  
         printf("Wrong with your input\n");  
         exit(0);  
       }   
     num=10*num+digit;  
   }  
   return num;  
}  
  
  
  
int getNumber(char a){ //converting char to number  
   if('0'<=a&&a<='9')  
     return a-'0';  
   else if ('a'<=a&&a<='f')  
     return 10+a-'a';  
   else if('A'<=a&&a<='F')  
     return 10+a-'A';  
   else{  
     printf("Wrong with your input\n");  
     exit(0);  
  }  
}   
  
int hex(char* str){ //converting string notation of hex number to decimal int  
   int num=0;  
   int i;  
   for(i=2;str[i]!='\0';i++){  
     int digit=getNumber(str[i]);  
       num=16*num+digit;  
   }  
   return num;  
}  
  
arg(char** args){  
   char* result;  
   int i;  
   if(args[1]==NULL){  
     printf("args=0");  
     return;  
   }  
   for(i=1;args[i]!=NULL;i++);  
   printf("argc=%d,args=",i-1);  
   for(i=1;args[i+1]!=NULL;i++)  
     printf("%s,",args[i]);  
   printf("%s\n",args[i]);  
}  
  
int search(char ** args)  //search the file you give , count the times of the string you have given   
{   
    char*str=args[1];   
    char*filename=args[2];   
    printf("search \"%s\" in file %s\n",str,filename);   
    int count=0;   
    int c;   
    int i=0;   
    int len=strlen(str);   
    FILE *fp;   
    fp=fopen(filename,"r");   
   
    while((c=fgetc(fp))!=EOF)   
    {   
        if(c==str[i])   
        {   
            if(++i==len)   
            count++;   
        }   
        else   
        {   
            i=0;   
        }   
        }   
    fclose(fp);   
    return count;   
}  
  