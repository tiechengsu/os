#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
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

void print_prinfo(struct prinfo* );
/*
 * Function: main()
 *
 * Description:
 *   Entry point for this program.
 *
 * Inputs:
 *   argc - The number of argument with which this program was executed.
 *   argv - Array of pointers to strings containing the command-line arguments.
 *
 * Return value:
 *   0 - This program terminated normally.
 */
int main (int argc, char ** argv) {
    struct prinfo *info;
    
    info = malloc(sizeof(struct prinfo));
    
    if (argc == 2) {
       info->pid = atoi(argv[1]);
    }
    else {
        printf("Error need one argument given: %d", argc);
        return 0;
    }

    syscall(181, info);
    print_prinfo(info);
    
    
    /* Exit the program */
    return 0;
}

void print_prinfo(struct prinfo* info) {

    printf("Value of state: %ld\n", info->state);
    printf("Value of pid: %d\n",info->pid);
    printf("Value of parent_pid: %d\n", info->parent_pid);
    printf("Value of youngest_child_pid: %d\n", info->youngest_child_pid);
    printf("Value of younger_sibling_pid: %d\n", info->younger_sibling_pid);
    printf("Value of older_sibling_pid: %d\n", info->older_sibling_pid);
    printf("Value of start_time: %ld\n", info->start_time);
    printf("Value of user_time: %ld\n", info->user_time);
    printf("Value of sys_time: %ld\n", info->sys_time);
    printf("Value of cutime: %ld\n", info->cutime);
    printf("Value of cstime: %ld\n", info->cstime);
    printf("Value of uid: %ld\n", info->uid);
    printf("Value of comm: %s\n", info->comm);
    printf("Value of signal: %ld\n", info->signal);
    printf("Value of num_open_fds: %ld\n", info->num_open_fds);

}
