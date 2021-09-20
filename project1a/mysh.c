#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
const int SIZE=512;

// error
void print_error_msg(){
  char error_message[30] = "An error has occurred\n";
  write(STDERR_FILENO, error_message, strlen(error_message));
}

void exec_common_shell(char * cmd[]){
  int pid=fork();
  if(pid==0){
   execvp(cmd[0],cmd);
   exit(0);
  }else if(pid>0) wait(&pid);
  else print_error_msg();
}

// cd
void change_directory(char * cmd []){
  char dest[SIZE];
  if(cmd[1]==NULL) strcpy(dest,getenv("HOME"));
  else strcpy(dest,cmd[1]);
  if(chdir(dest)==-1) print_error_msg();
}


// redirection
int find_redirection(char * cmd[],int _size){
  int i;
  for(i=0;i<_size-1;i++){
    if(strcmp(cmd[i],">")==0) return i;
  }
  return -1;
}

void redirection(char * cmd[],int mark){
  int pid=fork();
  if(pid==0){
       int fd=dup(STDOUT_FILENO);
       close(STDOUT_FILENO);
       int rfd=open(cmd[mark+1],O_CREAT | O_WRONLY | O_TRUNC);
       cmd[mark]=NULL;
       execvp(cmd[0],cmd);
       close(rfd);
       dup2(fd,STDOUT_FILENO);
       exit(0);
  }else if(pid>0) wait(&pid);
  else print_error_msg();
}


int main(int argc,char **argv){
  FILE * inFile;
  char * cmd [SIZE];
  int _size;
  char readin[SIZE];
  const char space[2]=" ";
  char * token;
    
  // 1. decide which stream to read in
  if(argc==1) inFile=stdin;
  else if(argc==2) inFile=fopen(argv[1],"r");
  else print_error_msg();
    
  while(!feof(inFile)){
 	if(argc==1) fprintf(stdout,"mysh>");	
	_size=0;
	
    	// 2. read from input stream and delete the enter in the end 
	fgets(readin,SIZE,inFile);
	if(strlen(readin)==0)continue;
	//fprintf(stdout,"readin: %s \n",readin);
	readin[strlen(readin)-1]='\0';
      
    	// 3. split the readin string with space and store into cmd.
	token = strtok(readin,space);
	while(token!=NULL){
	   cmd[_size++]=token;
	   token = strtok(NULL,space);
	   //fprintf(stdout,"cmd[%d] : %s\n",_size-1,cmd[_size-1]);
	}
	cmd[_size++]=NULL;
	
	// 4.execute commands.
	int redirection_mark=find_redirection(cmd,_size);
   	if(strcmp(cmd[0],"exit")==0) 
   		exit(0);
   	else if(strcmp(cmd[0],"pwd")==0) 
   		fprintf(stdout,"%s\n",getcwd(readin,SIZE));
   	else if(strcmp(cmd[0],"cd")==0)
   		change_directory(cmd);
   	else if(strcmp(cmd[0],"wait")==0) 
   		wait(NULL);
   	else if(redirection_mark!=-1)
   	   	redirection(cmd,redirection_mark);
        else exec_common_shell(cmd);
  }   
  return 0;
}  
