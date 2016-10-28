#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <linux/unistd.h>
#include <sys/syscall.h>

struct prinfo {
    long state; /* current state of process */
    pid_t pid; /* process id (input) */
    pid_t parent_pid; /* process id of parent */
    pid_t youngest_child_pid; /* process id of youngest child */
    pid_t younger_sibling_pid; /* pid of the oldest among younger siblings */
    pid_t older_sibling_pid; /* pid of the youngest among older siblings */
    unsigned long start_time; /* process start time */
    unsigned long user_time; /* CPU time spent in user mode */
    unsigned long sys_time; /* CPU time spent in system mode */
    unsigned long cutime; /* total user time of children */
    unsigned long cstime; /* total system time of children */
    long uid; /* user id of process owner */
    char comm[16]; /* name of program executed */
    unsigned long signal; /* The set of pending signals */
    unsigned long num_open_fds; /* Number of open file descriptors */
};


int main(int argc, char ** argv){
    
    struct prinfo* info;
    
    info = malloc(sizeof(struct prinfo));
    /*prints out its process ID*/
    info->pid = getpid();
    printf("pid: %d\n",info->pid);
    
    sigset_t base_mask;
    sigemptyset(&base_mask);
    
    /* set signals we want to block */
    sigaddset(&base_mask, SIGUSR2);
    sigprocmask(SIG_SETMASK, &base_mask, NULL);
    
    /*sends SIGUSR2 to itself two times*/
    kill(info->pid,SIGUSR2);
    kill(info->pid,SIGUSR2);
    
    /*stop the signal*/
    kill(info->pid, SIGTSTP);
    
    
    return 0;
}






