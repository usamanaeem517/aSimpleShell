#include<sys/types.h>
#include<iostream>
#include<unistd.h>
#include<sys/wait.h>
#include<stdio.h>
#include<cstdio> 
#include<string.h> 
#include<stdlib.h> 
#include<fcntl.h> 

#define LINE_LEN     80
#define MAX_ARGS     64
#define MAX_ARG_LEN  16
#define MAX_PATHS    64
#define MAX_PATH_LEN 96

#ifndef NULL
#define NULL ...
#endif
static int flag2 = 0;

struct command_t {
  char *name;
  int argc;
  char *argv[MAX_ARGS];
};

using namespace std;

void printPrompt() {
  static int flag = 1;
  if(flag){ 
   printf("\033[H\033[J");
   flag = 0;  
   cout<<endl;
   char buf1[1000];
   int fd = open("prompt.txt",O_RDONLY);
   read(fd, buf1, 1000);
   write(1,buf1,strlen(buf1));
   cout<<endl;   
  }
    char cwd[100]; 
    getcwd(cwd, sizeof(cwd));      
    cout<<getenv ("USER")<<"@"<<"ubuntu:~"<<cwd<<"$ ";
}

void readCommand(char * & buffer) {
  buffer = new char[100];
  cin.getline(buffer,100,'\n');
}

int parsePath(char ** & dirs) {
  dirs = new char *[64];
  char *pathEnvVar;
  char *thePath;
  for (int i=0; i < MAX_PATHS; i++)
  dirs[i] = NULL;
  pathEnvVar = (char *) getenv("PATH");
  thePath = (char *) malloc(strlen(pathEnvVar) + 1);
  strcpy(thePath, pathEnvVar);

  char temp[100];
  int k=0;
  int n=0; 
  for(int i=0; i < strlen(thePath); i++){ 
   while(thePath[i]!=':' && i < strlen(thePath)){
        temp[k]=thePath[i];
        temp[k+1]='\0';
        k++;
        i++;
    }
 if(thePath[i]==':'){
   dirs[n] = new char [strlen(temp)+1]; 
   strcpy(dirs[n],temp);
   n++;
   }
   k=0;
  }
  char * temp2 = (char *) getenv("PWD");
  dirs[n] = new char [strlen(temp2)+1]; 
  strcpy(dirs[n],temp2);
}

char *lookupPath(char **argv, char **dir) {
  char *result;
  char *pName=NULL;

  if (*argv[0] == '/') {
    result =  new char[strlen(argv[0])+1];
   strcpy(result,argv[0]);
   return result;
  }

  for (int i = 0; dir[i]!=NULL ; i++) {
     pName = new char [MAX_PATH_LEN];
     strcpy(pName,dir[i]);
     int size = strlen(pName);
      pName[size]='/';
      for(int i=size+1, j=0; argv[0][j]!='\0' ; i++,j++){         
         pName[i]=argv[0][j];
         pName[i+1]='\0';  
 } 
     int fd = access(pName,F_OK); 
     if(fd!=-1){
    return pName;   
}   
  }

  if(flag2==0){
  fprintf(stderr, "%s: command not found\n", argv[0]);
 }
  return NULL;
}

int parseCommand(char *line, struct command_t * cmd){
 int n=0;
 int j=0; 
 int m=0;
 char com[100];
 com[0]='\0';   
while(line[n]!='\0'){ 
   while(line[n]!=' ' && line[n]!='\0'){
	  com[j]=line[n];
	  com[j + 1] = '\0';
	  n++;
          j++;
	 }
         if(line[n]==' ' || line[n]=='\0'){
           cmd->argv[m] = new char[strlen(com)+1];
           strcpy(cmd->argv[m],com);                 
           m++;
           if(line[n]==' ')
           n++;
           j=0;
         }
}
 cmd->argv[m]=NULL;
 if(strcmp(com,"exit")==0){    
     exit(0);
  }
 else if(strcmp(cmd->argv[0],"cd")==0 && cmd->argv[1]!=NULL){
     chdir(cmd->argv[1]);
     flag2=1;
 }
 else if(strcmp(cmd->argv[0],"cd")==0 && cmd->argv[1]==NULL){
    char * dir = getenv ("USER");
    char real[30] = "/home/";     
    strcpy(real+strlen(real),dir);
    chdir(real);
    flag2=1; 
 }
}

int main() {
   
 char ** temp2; 
 parsePath(temp2); 
 char * commandline;
while(true){
 printPrompt();  
 readCommand(commandline);
 command_t command;
 parseCommand(commandline, &command); 
    command.name = lookupPath(command.argv, temp2);
    if (command.name == NULL && flag2 == 0) {
      cout<<"Command Not Right!!"<<endl;
      // continue;
    }
else {
 pid_t pid = fork();
    if(pid == 0){
           execv(command.name,command.argv);
   }  
    else if(pid > 0){  
         wait(NULL);
   } 
  }
flag2=0;
}

return 0;
}
