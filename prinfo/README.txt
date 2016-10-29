git clone https://github.com/jtcriswell/linux256.git
make mrproper
cp csc256.config .config
make bzImage -j 10 $-j compile files in parallel
sh runqemu.sh linux-3.18.42/arch/x86_64/boot/bzImage#boot QEMU
uname -a #check if booted the correct kernel image
poweroff #shutdown
mount -t msdos /dev/fd0 /mnt #importing files into the QEMU virtual machine

************* Files modified or added to the kernel source code *******
1. Modify “syscall_64.tbl” in linux-3.18.42/arch/x86/syscalls
2. Add “prinfo.h” to ~/linux256/linux-3.18.42/include/linux
3. Creat prinfo folder in ~/linux256/linux-3.18.42 and add “prinfo.c”,”Makefile”
4. Modify “Makefile” in ~/linux256/linux-3.18.42
5. Modify “syscalls.h” in ~/linux256/linux-3.18.42/include/linux


************* Job Description ********************
Tiecheng Su:
implement “state”,”pid”,”parent_pid”,“youngest_child_pid”, “younger_sibling_pid”,”older_sibling” also test, exp1, exp2 and exp3.

Yiming Huang:
modify “syscalls_64.tbl”,”syscalls.h”,“Makefile”, add“prinfo.h”
fetch “uid”,”start_time”,”user_time”,”sys_time”,”cutime”,”cstime”,”comm”,”num_open_fds”,”signal”.


************* Experiments Results ****************
1.
Value of state: 1
Value of pid: 561
Value of parent_pid: 124
Value of youngest_child_pid: -1
Value of younger_sibling_pid: 562
Value of older_sibling_pid: -1
Value of start_time: 45124373631
Value of user_time: 13000000
Value of sys_time: 95000000
Value of cutime: 0
Value of cstime: 0
Value of uid: 0
Value of comm: exp1
Value of signal: 0
Value of num_open_fds: 3
state represents the state of the process, -1 unrunnable, 0 runnable, >0 stopped
In this experiments, the process is put to sleep, so the state is 1

2.
Value of state: 0
Value of pid: 563
Value of parent_pid: 124
Value of youngest_child_pid: -1
Value of younger_sibling_pid: -1
Value of older_sibling_pid: -1
Value of start_time: 357271484521
Value of user_time: 12000000
Value of sys_time: 20000000
Value of cutime: 0
Value of cstime: 0
Value of uid: 0
Value of comm: exp2
Value of signal: 0
Value of num_open_fds: 3

Value of state: 0
Value of pid: 564
Value of parent_pid: 124
Value of youngest_child_pid: -1
Value of younger_sibling_pid: -1
Value of older_sibling_pid: -1
Value of start_time: 369302604237
Value of user_time: 2000000
Value of sys_time: 10000000
Value of cutime: 0
Value of cstime: 0
Value of uid: 0
Value of comm: exp2
Value of signal: 0
Value of num_open_fds: 3

Value of state: 0
Value of pid: 565
Value of parent_pid: 124
Value of youngest_child_pid: -1
Value of younger_sibling_pid: -1
Value of older_sibling_pid: -1
Value of start_time: 373003516467
Value of user_time: 0
Value of sys_time: 9000000
Value of cutime: 0
Value of cstime: 0
Value of uid: 0
Value of comm: exp2
Value of signal: 0
Value of num_open_fds: 3

state=0: means the process is currently running
parent_pid = 124: all the process has the same parent with pid 124
youngest_child_pid, younger_child_pid, older_sibling_pid = -1 means current process doesn’t has any child or siblings
start_time: u64 value of the jiffies when the task was created
user_time: CPU time spent in user mode
sys_time: CPU time spent in system mode
cutime: total user time of children
cstume: total system time of children
uid: user id of process owner, 0 means own by the root
comm[16]: name of program executed
signal: the set of pending signals
num_open_fds: number of open file descriptors

3.
Value of state: 132
Value of pid: 394
Value of parent_pid: 123
Value of youngest_child_pid: -1
Value of younger_sibling_pid: 467
Value of older_sibling_pid: -1
Value of start_time: 27975682577
Value of user_time: 13000000
Value of sys_time: 85000000
Value of cutime: 0
Value of cstime: 0
Value of uid: 0
Value of comm: exp3
Value of signal: 2048
Value of num_open_fds: 3

The value is 2048
The reason why signals cannot be queued is that Linux system use an array to store pending signal. If the signal is pending, the corresponding bit is set to 1. Otherwise, it’s 0. Therefore, pending signals of the same type are not queued, but discarded.





