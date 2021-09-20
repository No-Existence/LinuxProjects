## Basic Shell

### Deal with string

```C
#include<string.h>
char *strcpy(char *dest, const char *src);
char *strtok(char *str, const char *delim);
```

Usage of `strtok`

```c
#include <string.h>
#include <stdio.h>
 
int main () {
   char str[80] = "This is - www.runoob.com - website";
   const char s[2] = "-";
   char *token;
   /* 获取第一个子字符串 */
   token = strtok(str, s);
   /* 继续获取其他的子字符串 */
   while( token != NULL ) {
      printf( "%s\n", token );
      token = strtok(NULL, s);
   }
   return(0);
}
```



### Deal with process

```C
#include<sys/types.h>
#include<sys/wait.h>
pid_t wait (int * status);

#include <unistd.h>
int execvp(const char * cmd, char * argv[]);
pid_t fork(void);
```

Usage of `execvp`

```c
char * cmd[3];
cmd[0]="cat";
cmd[1]="file.c";
cmd[2]=NULL;
execvp(cmd[0],cmd);
```



### Version 1

```C
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
const int SIZE=512;

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
    
  while(1){
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
   	if(strcmp(cmd[0],"exit")==0)break;	
      
    // 4. create a process to execute the command
    // Note that the father process must wait until son process has exited.
	int pid=fork();
	if(pid==0){
	   execvp(cmd[0],cmd);
	   exit(0);
	}else if(pid>0) wait(&pid);
  }   
  return 0;
}  
```



## Built-in Commands



```C
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <stdlib.h>

// get current working directory
char * getcwd(char * buf,size_t size);

// change directory
int chdir(const char * path); // 0 success, -1 fail

// get the value of environment variable in linux.
char * getenv(char *envvar);  
```



### Version 2

```C
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
const int SIZE=512;

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
    
  while(1){
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
   	if(strcmp(cmd[0],"exit")==0) exit(0);
   	else if(strcmp(cmd[0],"pwd")==0) fprintf(stdout,"%s\n",getcwd(readin,SIZE));
   	else if(strcmp(cmd[0],"cd")==0){
   		char dest[SIZE];
   		if(cmd[1]==NULL) strcpy(dest,getenv("HOME"));
   		else strcpy(dest,cmd[1]);
   		int r=chdir(dest);
   		if(r==-1) perror("change directory error.");
   	}else if(strcmp(cmd[0],"wait")==0) wait(NULL);
      
    	// 4. create a process to execute the command
	int pid=fork();
	if(pid==0){
	   execvp(cmd[0],cmd);
	   exit(0);
	}else if(pid>0) wait(&pid);
  }   
  return 0;
}  
```



## Redirection



```c
#include <unistd.h>
int dup(int oldfp); // copy a new file description of oldfp
int dup2(int oldfp,int newfd); // copy and then give it a new fd.
```

Usage

```c
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>

void file_Redirect()
{
    //先备份现场
    int outfd = dup(1);
    //先做重定向
    int fd = open("world", O_WRONLY|O_CREAT,0666);
    //标准输出到重定向fd到对应的文件
    dup2(fd, 1);
    printf("hello Linux\n");
    //需要来一次刷新
    fflush(stdout);
    //需要恢复1，重新到标准输出
    dup2(outfd, 1);
    printf("hello Linux\n");
}
int main(int argc, char* argv[])
{
    file_Redirect();
    return 0;
}
```

### Version 3

With Redirection

```c
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
const int SIZE=512;

int find_redirection(char * cmd[],int _size){
  int i;
  for(i=0;i<_size-1;i++){
    if(strcmp(cmd[i],">")==0) return i;
  }
  return -1;
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
    
  while(1){
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
	int redirection_mark=find_redirection(cmd,_size);
   	if(strcmp(cmd[0],"exit")==0) exit(0);
   	else if(strcmp(cmd[0],"pwd")==0) fprintf(stdout,"%s\n",getcwd(readin,SIZE));
   	else if(strcmp(cmd[0],"cd")==0){
   		char dest[SIZE];
   		if(cmd[1]==NULL) strcpy(dest,getenv("HOME"));
   		else strcpy(dest,cmd[1]);
   		int r=chdir(dest);
   		if(r==-1) perror("change directory error.");
   	}else if(strcmp(cmd[0],"wait")==0) wait(NULL);
   	else if(redirection_mark!=-1){
   	   int pid=fork();
   	   if(pid==0){
   	     int fd=dup(STDOUT_FILENO);
   	     close(STDOUT_FILENO);
   	     open(cmd[redirection_mark+1],O_CREAT | O_RDWR);
   	     cmd[redirection_mark]=NULL;
   	     execvp(cmd[0],cmd);
   	     exit(0);
   	    }else if(pid>0) wait(&pid);
        }else {
    	// 4. create a process to execute the command
	int pid=fork();
	if(pid==0){
	   execvp(cmd[0],cmd);
	   exit(0);
	}else if(pid>0) wait(&pid);
	}
  }   
  return 0;
}  
```

