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
