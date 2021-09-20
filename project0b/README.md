[![Open in Visual Studio Code](https://classroom.github.com/assets/open-in-vscode-f059dc9a6f8d3a56e377f745f24479a46679e63a5d9fe6f495e02850cd0d8118.svg)](https://classroom.github.com/online_ide?assignment_repo_id=5618189&assignment_repo_type=AssignmentRepo)



First, write a program that can read from one file and write into another. Make sure that this runs correctly.

```C
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>

int main(int argc,char ** argv){
   if(argc!=3){
     perror(" Argument Error : Please provide two filepaths ");
     exit (1);
   }
   int infile = open(argv[1], O_RDONLY  );
   int outfile = open(argv[2], O_CREAT | O_WRONLY | O_TRUNC );
   const int SIZE = 4;
   char buf[SIZE];
   int c; 
   while((c=read(infile,buf,SIZE))>0){
      if((c=write(outfile,buf,SIZE))<0){
        perror(" Write Error ");
        exit (1);
      }
   }
   return 0;
}  
```

Usage of  stat

```
#include <sys/types.h>
#include <sys/stat.h>
int stat(const char * pathname, struct stat * buf);
```

Usage of open , close , read and write

```
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

int open(const char * path,int oflags);
// oflags : O_RDONLY  O_WRONLY  O_RDWR  O_TRUNC O_APPEND O_CREAT

int close(int fildes);

size_t read(int fildes,void *buf,size_t nbytes);

size_t write(int filedes, const void * buf,size_t nbytes);
```



Final version

```C
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include "sort.h"

void printRec(rec_t * r ,int n){
   int i;
   for(i=0;i<n;i++){
     printf("%d:%d\n",r[i].key,r[i].record);
   }
 }

int rec_t_cmp(const void * r1, const void * r2){
    rec_t * rt1 = (rec_t*)r1;
    rec_t * rt2 = (rec_t*)r2;
    return rt1->key -rt2->key ;
}

int main(int argc,char ** argv){
   if(argc!=3){
     fprintf(stderr,"Usage: fastsort inputfile outputfile");
     exit (1);
   }
   int infile,outfile; 
   if((infile = open(argv[1], O_RDONLY))<0){
     fprintf(stderr,"Error: open file %s\n",argv[1]);
     exit(1);
   } 
   if((outfile = open(argv[2], O_CREAT | O_WRONLY | O_TRUNC ))<0){
     fprintf(stderr,"Error: open file %s\n",argv[2]);
     exit(1);
   } 
   int c,i,n; 
   struct stat buf;
   stat(argv[1],&buf);   
   n=buf.st_size/100;      
   rec_t * r = (rec_t*)malloc(n*sizeof(rec_t));
   
   for(i=0;i<n;i++){
     c= read(infile,&r[i],sizeof(rec_t));
     if(c<0) fprintf(stderr,"Read Error"); 
   }
   qsort(r,n,sizeof(rec_t),rec_t_cmp);
//   printRec(r,n);
   for(i=0;i<n;i++){
     c=write(outfile,&r[i],sizeof(rec_t));
     if(c<0) fprintf(stderr,"Write Error");
   } 
   close(infile);
   close(outfile);
   free(r);
   return 0;
}  
```

Run

```
gcc generate.c -o generate 
./generate -s 0 -n 100 -o input.txt
touch output.txt
gcc fastsort.c -o fastsort
./fastsort input.txt output.txt
```

