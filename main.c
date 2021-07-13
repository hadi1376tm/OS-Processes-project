#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdbool.h>
#include <errno.h>
  #define STB_IMAGE_IMPLEMENTATION
  #include "stb_image/stb_image.h"
  #define STB_IMAGE_WRITE_IMPLEMENTATION
  #include "stb_image/stb_image_write.h"
  
int main(void) {
    int Max_ch_len=200;
    bool flag=false;
    char addr[]= "sky.bmp";
    int len = strlen(addr);
    int fd[2];
    int fd2[2];
    if(pipe(fd)== -1){
        printf("pipe failed");
        return 1;
        
    }
    if(pipe(fd2)== -1){
        printf("pipe failed");
        return 1;
        
    }
   
    
    int f1 = fork();
     if(f1<0){
         printf("error in fork");
    }
    else
    {
        if(f1==0){
            sleep(1);
          printf("\nchild A\n");    ///// child A (histogram)
          char addr2;
          int addr_fd= open("address_fifo", O_RDONLY);  
          if(read(addr_fd,&addr2,len)==-1)
                {
                    printf("Error in read address in A\n");
                    exit(1);
                }
                else{
         printf("(%s) success read address in A \n",&addr2);
        
          close(addr_fd);
          // exec hist
          sleep(2);
           char *args[]={&addr2,NULL};
           execv("./hist",args);
          // exec hist
                }
                sleep(8);
        if(flag){ // run for filterd img
            sleep(2);
          int addr_fd= open("address_fifo", O_RDONLY);  
          if(read(addr_fd,&addr2,len)==-1)
                {
                    printf("Error in read address guassian in A\n");
                    exit(1);
                }
                
         printf("(%s) success read address in A \n",&addr2);
        
          close(addr_fd);
          // exec hist
          printf("before");
           char *args[]={&addr2,NULL};
           execv("./hist",args);
          // exec hist
                
        }
        }
        else{
            int f2=fork();
            if(f2==0)
            {
                sleep(1);
                printf("child B\n");   /// child B (gaussian filtering)
                char addr3;
                int addr_fd3= open("address_fifo2", O_RDONLY);
                if(read(addr_fd3,&addr3,len)==-1)
                {
                    printf("Error in read address in B\n");
                    exit(1);
                }
                else{
                    printf("(%s) success read address in B \n",&addr3);
        
                    close(addr_fd3);
                    // exec filtering
                    char *args2[]={"1",&addr3,NULL};
                    execv("./gfilter",args2);
                }
            }
            else{
                int histogram[256]={0};
                int f3=fork();
                if(f3==0){
                printf("\nchild C\n");  /////////////////////////////////////// child C
                //read parent to C
                close(fd[1]);
                char addr;
          
                if(read(fd[0],&addr,len)==-1)
                {
                    printf("Error in read address in C\n");
                    exit(1);
            }
            else{
                    printf("(%s) success read address in C \n",&addr);
                    close(fd[0]);
                
            }
                    // fifo C to A 
                    if(mkfifo("address_fifo", 0777)==-1){
                        if(errno != EEXIST){
                        printf("could not create fifo in C\n");
                        exit(1);
                        }
                    }
                    int addr_fd= open("address_fifo", O_WRONLY);
                    if(write(addr_fd,&addr,strlen(&addr))==-1){
                        printf("could not write fifo in C\n");
                        exit(1);
                    }
                    close(addr_fd);
                    
                    // fifo C to  B
                    if(mkfifo("address_fifo2", 0777)==-1){
                        if(errno != EEXIST){
                        printf("could not create fifo in C\n");
                        exit(1);
                        }
                    }
                    int addr_fd2= open("address_fifo2", O_WRONLY);
                    if(write(addr_fd2,&addr,strlen(&addr))==-1){
                        printf("could not write fifo in C\n");
                        exit(1);
                    }
                    close(addr_fd2);
                    
                    /// fifo from hist
                    sleep(5);
                    int hist_fd= open("histfifo", O_RDONLY);
                    if(read(hist_fd,&histogram,sizeof(histogram))==-1)
                    { 
                        printf("could not open histfifo in C\n");
                        exit(1);
                        
                    }
                    close(hist_fd);
                
                    // histogram to parent from C 
                    sleep(1);
                    close(fd2[0]);
                    if(write(fd2[1],&histogram,sizeof(histogram))==-1){
                    printf("Error in fifo from C to parent\n");
                    exit(1);
                    }
                    printf("success write histo pipe C to parent\n");
                    close(fd2[1]);
                    
                    // fifo guassian filterd img address from gfilter
                    sleep(2);
                    char addrg;
                    int addr_fd_g= open("g_fifo", O_RDONLY);  
                    if(read(addr_fd_g,&addrg,Max_ch_len)==-1)
                    {
                        printf("Error in read address in A\n");
                        exit(1);
                    }
                else{ //second time for gussian img
                    close(fd[0]);
                    if(write(fd[1],&addrg,strlen(addrg))==-1){
                    printf("Error in pipe parent\n");
                    exit(1);
                    }
                printf("(send %s)success write pipe parent to parent\n",&addr);
                close(fd[1]);
                    flag=true;
                    printf("flag now is true\n");
                    printf("(%s) success read guassian address in C \n",&addrg);
                    close(addr_fd_g);
                    // send gaussian adress to A
                     if(mkfifo("address_fifo", 0777)==-1){
                        if(errno != EEXIST){
                        printf("could not create fifo in C\n");
                        exit(1);
                        }
                    }
                    int addr_fd= open("address_fifo", O_WRONLY);
                    if(write(addr_fd,&addrg,strlen(&addrg))==-1){
                        printf("could not write fifo in C\n");
                        exit(1);
                    }
                    // get histogram gaussian img
                    close(addr_fd);
                    int histogram3[256]={0};
                    sleep(2);
                    int hist_fd= open("histfifo", O_RDONLY);
                    if(read(hist_fd,&histogram3,sizeof(histogram3))==-1)
                    { 
                        printf("could not open histfifo in C\n");
                        exit(1);
                        
                    }
                    close(hist_fd);
                     // histogram to parent from C 
                    sleep(3);
                    close(fd[0]);
                    if(write(fd[1],&histogram3,sizeof(histogram3))==-1){
                    printf("Error in fifo from C to parent\n");
                    exit(1);
                    }
                    printf("success write histo pipe C to parent\n");
                    close(fd[1]);
                
                }
                }
                else{
                    
                printf("\nparent\n");  /////////// //////parent
                close(fd[0]);
                if(write(fd[1],&addr,len)==-1){
                printf("Error in pipe parent\n");
                exit(1);
                }
                printf("(send %s)success write pipe parent to C\n",&addr);
                close(fd[1]);
                sleep(5);
                int histogram2[256]={0};
                int histogram4[256]={0};
                close(fd2[1]);
          
                if(read(fd2[0],&histogram2,sizeof(histogram2))==-1)
                {
                    printf("Error in read histogram from C in parent\n");
                    exit(1);
            }
            else{
                    printf(" success read histogram from C in parent \n");
                    close(fd2[0]);
                
            }
                sleep(5);
                printf("orginal img: \n");
                for(int i=0 ; i<257; i++)  //show histogram
                {
                    printf("%d value = %d pixels \n",i,histogram2[i]);
                }
                printf("gausian image saved location: ./%s",&addr);
                ///////////////////// filterd img histogram show
                sleep(5);
                 printf("\n\n\n\ngaussian filtered img: \n");
                close(fd[1]);
          
                if(read(fd[0],&histogram4,sizeof(histogram4)-1)==-1)
                {
                    printf("Error in read histogram from C in parent2\n");
                    exit(1);
            }
            else{
                    printf(" success read histogram from C in parent \n");
                    close(fd[0]);
                
            }
                
                for(int i=0 ; i<257; i++)  //show histogram
                {
                    printf("%d value = %d pixels \n",i,histogram4[i]);
                }

                //close(fd3[1]);
                //read(fd3[0],&histogram2,strlen(addr));
                //close(fd3[0]);
                //printf("%d",histogram2[10]);
                }
            }
        }
        
    }
     
}

// TODO show guassian file locatin
// TODO show guassian histogram
