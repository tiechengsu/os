//
//  test3.c
//  
//
//  Created by Harry Su on 11/27/16.
//
//

/*
 *uses mmap() to map two different files into two different virtual addresses; call these virtual addresses A
 *and B. This program should then use readMMU() and writeMMU() to map the physical page mapped to virtual 
 *address A to virtual address B.
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/syscall.h>

int main(int argc, char *argv[]){
    struct stat sbA,sbB;
    int fdA,fdB;
    volatile unsigned long *vaddrA, *vaddrB;
    unsigned long pteA = 0, pteB = 0;
    
    if(argc != 3){
        fprintf(stderr, "Usage: %s <pathname><pathname>\n",argv[0]);
        exit(EXIT_FAILURE);
    }
    /*open file A*/
    fdA = open(argv[1],O_RDWR);
    if(fdA==-1){
        perror("open");
        exit(EXIT_FAILURE);
    }
    
    /* To obtain file size */
    if(fstat(fdA,&sbA)){
        perror("fstat");
        exit(EXIT_FAILURE);
    }
    /*creates a new mapping in the virtual address space */
    vaddrA = mmap(NULL,sbA.st_size,PROT_READ|PROT_WRITE,MAP_SHARED,fdA,0);
    if(vaddrA==MAP_FAILED){
        perror("mmap");
        exit(EXIT_FAILURE);
    }
    
    /*open file B*/
    fdB = open(argv[2],O_RDWR);
    if(fdB==-1){
        perror("open");
        exit(EXIT_FAILURE);
    }
    /* To obtain file size */
    if(fstat(fdB,&sbB)){
        perror("fstat");
        exit(EXIT_FAILURE);
    }
    /*creates a new mapping in the virtual address space */
    vaddrB = mmap(NULL,sbB.st_size,PROT_READ|PROT_WRITE,MAP_SHARED,fdB,0);
    if(vaddrB==MAP_FAILED){
        perror("mmap");
        exit(EXIT_FAILURE);
    }
    
    /*try to read the mapped file*/
    printf("vaddrA: %lu\n",*vaddrA);
    printf("vaddrB: %lu\n",*vaddrB);
    
    
    
    if (syscall(181, vaddrA, &pteA)) {
        printf("readMMU error\n");
        return -1;
    }
    
    if (syscall(181, vaddrB, &pteB)) {
        printf("readMMU error\n");
        return -1;
    }
    
    
    /* map the physical page mapped to virtual address A to virtual address B */
    if (syscall(182, vaddrB, pteA)) {
        printf("writeMMU error\n");
        return -1;
    }
    printf("virtual address B has been changed\n");
    
    /* write into virtual address A*/
    *vaddrA = 3;
    /* check if the same value written into virtual address B*/
    printf("vaddrA: %lu\n",*vaddrA);
    printf("vaddrB: %lu\n",*vaddrB);
    
    /* restore the pte of file B */
    if (syscall(182, vaddrB, pteB)) {
        printf("writeMMU error\n");
        return -1;
    }
    
    return 0;
    
}
