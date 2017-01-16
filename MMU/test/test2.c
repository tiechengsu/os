//
//  test2.c
//  
//
//  Created by Harry Su on 11/26/16.
//
//
/*
 *uses mmap() to map a file into memory and then calls readMMU() to find the page table entry for the virtual
 *address to which the file is mapped.
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
    struct stat sb;
    int fd;
    unsigned long *vaddr;
    unsigned long pte = 0;
    
    if(argc != 2){
        fprintf(stderr, "Usage: %s <pathname>\n",argv[0]);
        exit(EXIT_FAILURE);
    }
    
    fd = open(argv[1],O_RDWR);
    if(fd==-1){
        perror("open");
        exit(EXIT_FAILURE);
    }
    
    /* To obtain file size */
    if(fstat(fd,&sb)){
        perror("fstat");
        exit(EXIT_FAILURE);
    }
    /*creates a new mapping in the virtual address space */
    vaddr = mmap(NULL,sb.st_size,PROT_READ|PROT_WRITE,MAP_SHARED,fd,0);
    if(vaddr==MAP_FAILED){
        perror("mmap");
        exit(EXIT_FAILURE);
    }
    
    printf("Call readMMU after mmap:\n");
    
    if (syscall(181, vaddr, &pte)) {
        printf("readMMU error\n");
    }
    printf("pte : 0x%lx\n",pte);
    /* try to access the mapped file */
    unsigned long access = *vaddr;
    printf("Call readMMU after access it:\n");
    
    if (syscall(181, vaddr, &pte)) {
        printf("readMMU error\n");
        return -1;
    }
    printf("pte = 0x%lx\n", pte);
    
    return 0;
}


