#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <linux/unistd.h>
#include <sys/syscall.h>


int main (int argc, char ** argv) {
    pid_t pid = getpid();
    printf("pid: %d\n",pid);
    sleep(100);
    return 0;
}
