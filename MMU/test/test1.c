//
//  test1.c
//  
//
//  Created by Harry Su on 11/26/16.
//
//

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/syscall.h>

/* reads the page table entry for virtual address zero */
int main(int argc, char *argv[]){
    unsigned long pte = 0;
    if(syscall(181,0,&pte)){
        printf("readMMU error\n");
    }
    if(syscall(182,0,pte)){
        printf("writeMMU error\n");
        return -1;
    }
    printf("pte : %lx\n",pte);
    return 0;
}
