#include <linux/kernel.h>
#include <linux/prinfo.h>
#include <linux/syscalls.h>
#include <linux/uaccess.h>
#include <linux/pid.h>
#include <linux/sched.h>
#include <linux/slab.h>
#include <asm/errno.h>
#include <linux/fdtable.h>
#include <linux/signal.h>
#include <linux/times.h>
#include <asm/unistd.h>
#include <asm/current.h>
#include <linux/types.h>
#include <linux/list.h>


/*helper function for sys_prinfo see their full decleartions
   for further description about them*/
pid_t find_youngest_child_pid(struct task_struct *task);
pid_t find_younger_sibling_pid(struct task_struct *task);
pid_t find_older_sibling_pid(struct task_struct *task);
unsigned long get_children_user_time(struct task_struct *task);
unsigned long get_children_sys_time(struct task_struct *task);
unsigned long number_open_fds(struct files_struct *files);


/*
 * System call: sys_prinfo
 * Number: 181
 *
 * Description:
 *  takes in a struct prinfo pointer that should contain a pid
 *  then fills in the fields of prinfo from the task_struct
 *  that corresponds to the given pid
 *
 * Inputs:
 *  info - struct prinfo pointer supplied by user to be filled in
 *
 *  Return values:
 *      EINVAL - info was null, or there was no task_struct that matched
 *               the given pid
 *      EFUALT - info pointed to faulty memory
 *      0      - succesful execution
 * */



SYSCALL_DEFINE1(prinfo, struct prinfo *, info) {
    struct prinfo *k_info;
    struct task_struct *task;
    
    
    
    /*return EINVAL if the input prinfo structure is NULL*/
    if (info == NULL) {
        return EINVAL;
    }
    /*return EFAULT id allocation failed*/
    k_info = kmalloc(sizeof(struct prinfo), GFP_KERNEL); /*This is the prinfo struct the kernel will fill out*/
    if(k_info == NULL){
        return EFAULT;
    }
    /*validate pointer from user*/
    if (copy_from_user(k_info, info, sizeof(struct prinfo)) != 0){
        return EFAULT;
    }
    task = get_pid_task(find_get_pid(k_info->pid),PIDTYPE_PID);
   
    /*process ID queried does not exist*/
    if (task == NULL) {
        return EINVAL;
    }
    
    /*fill in fields from task may have to call helepr functions*/
    k_info->state = task->state;
    
    k_info->pid = task->pid;

    k_info->parent_pid = task->parent->pid;
    
    k_info->youngest_child_pid = find_youngest_child_pid(task);
    
    k_info->younger_sibling_pid  = find_younger_sibling_pid(task);
   
    k_info->older_sibling_pid = find_older_sibling_pid(task);
    
    k_info->start_time  = task->start_time;
    
    k_info->user_time = task->utime;
    
    k_info->sys_time = task->stime;
    
    k_info->cutime = get_children_user_time(task);
    
    k_info->cstime = get_children_sys_time(task);

    k_info->uid = (long) task->cred->uid.val;
    
    /*command name, we need to truncate the program name to 15 bytes and 
     *unconditionally add a string terminator byte to the comm field
     */
    strncpy(k_info->comm, task->comm, 15);
    k_info->comm[15] = '\0';
    
    k_info->signal = task->signal->shared_pending.signal.sig[0];
    
    k_info->num_open_fds = number_open_fds(task->files);
    
    if(copy_to_user(info, k_info, sizeof(struct prinfo))){
        return EFAULT; 
    }
    kfree(k_info);
    
    return 0;
}

/*
 * Funtion: number_open_fds
 * 
 * Description:
 *  returns number of open file descriptors from the given files_struct
 *
 * Inputs:
 *  files - A pointer to a files_struct containing the open files to be
 *  enumerated
 *
 *  Return value:
 *    i - a long representing the number of open file descriptors
 *
 */


unsigned long number_open_fds(struct file_struct *files){
    int i = 0;
    unsigned long t = 0;
    int count = 0;
    struct fdtable *fdt;
    fdt = files_fdtable(files);
    int n = fdt->max_fds/BITS_PER_LONG;
    unsigned long *open_fd = fdt->open_fds;
    /* each bit in t represents whether file is open */
    for(int i=0; i<n; i++){
        t = open_fd[i];
        while(t>0){
            count += t&1;
            t>>=1;
        }
    }
    return count;
}

/*
 * Function: find_youngest_child_pid
 *
 * Desicription:
 *  returns pid of youngest child in the given task_struct
 *
 * Inputs:
 *   task - pointer to a task_struct to find the youngest child of
 *
 *  Return value:
 *    pid of the youngest child or -1 if there were no children of the given process
 *  */
pid_t find_youngest_child_pid(struct task_struct *task) {
    struct task_struct *cur, *cur_youngest = NULL;
   
    list_for_each_entry(cur, &(task->children),sibling) {
        if(cur->parent != task) continue; //sanity check
        if(timespec_compare(&cur->start_time,&cur_youngest->start_time)>0){
            cur_youngest = cur;
        }
    }

    return cur_youngest? cur_youngest->pid:-1;
}

/*
 * Function: find_younger_sibling_pid
 *
 * Description:
 *  returns pid of oldest amongst the younger siblings
 *
 * Inputs:
 *   task - pointer to a task_struct to find the younger sibling
 *
 * Return value:
 *  pid of the oldest amongth the younger siblings
 *  of the supplied task_struct
 */
pid_t find_younger_sibling_pid(struct task_struct *task) {
    struct task_struct *cur, *cur_younger = NULL;
   
    list_for_each_entry(cur, &(task->sibling),sibling) {
        if(cur->parent != task->parent) continue; //sanity check
        if(timespec_compare(&cur->start_time,&task->start_time)>0 && (cur_younger? timespec_compare(&cur->start_time,&cur_younger->start_time)<0:true)) {
            cur_younger = cur;
        }
    }
    
    return cur_younger? cur_younger->pid:-1;
}

/*
 * Function: find_older_sibling_pid
 *
 * Description:
 *  returns pid of youngest amongst the older siblings
 *
 * Inputs:
 *   task - pointer to a task_struct to find older sibling
 *
 * Return value:
 *  pid of the youngest amongth the older siblings
 *  of the supplied task_struct
 */
pid_t find_older_sibling_pid(struct task_struct *task) {
    struct task_struct *cur, *cur_older = NULL;
    
    list_for_each_entry(cur, &task->sibling,sibling) {
        if(cur->parent != task->parent) continue; //sanity check
        if(timespec_compare(&cur->start_time,&task->start_time)<0 && (cur_older? timespec_compare(&cur->start_time,&cur_older->start_time)>0:true)) {
            cur_older = cur;
        }
    }
    
    return (cur_older)? cur_older->pid:-1;
}

/*
 * Function: get_children_user_time
 *
 * Description:
 *  returns the user time of all children
 *
 * Inputs: 
 *  task - pointer to a task_struct to find the aggregate children user time
 *
 * Return value:
 *   children_user_time - an unsigned long of all children user time   
 */
unsigned long get_children_user_time(struct task_struct *task) {
    struct task_struct *current_child;
    unsigned long children_user_time = 0;
    struct list_head *list;
    
    list_for_each(list, &task->children) {
        current_child = list_entry(list, struct task_struct, sibling);
        children_user_time += current_child->utime;    
    }
    
    return children_user_time;
}    

/*
 * Function: get_children_system_time
 *
 * Description:
 *  returns the system time of all children
 *
 * Inputs: 
 *  task - pointer to a task_struct to find the aggregate children system time
 *
 * Return value:
 *   children_system_time - an unsigned long of all children user time   
 */
unsigned long get_children_sys_time(struct task_struct *task) {
    struct task_struct *current_child;
    unsigned long children_sys_time = 0;
    struct list_head *list;
    
    list_for_each(list, &task->children) {
        current_child = list_entry(list, struct task_struct, sibling);
        children_sys_time += current_child->stime;    
    }
    
    return children_sys_time;
}
