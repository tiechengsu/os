(1)eval: Parse and run the command line.
(2)builtin_cmd: Recognizes built-in functions including exit, cd and jobs.
(3)do_bgfg: Implements built-in bg and fg functions.
(4)waitfg: Wait for the foreground process to terminate.
(5)do_pipe: Allow the standard inputs and standard outputs of a list of programs to be concatenated in a chain.
(6)sigchld_handler: Terminated or stopped child
(7)sigint_handler: Sends a SIGINT to the shell whenever the user types ctrl-c, catch it and send it to foreground.
(8)sigtstp_handler: Sends a SIGTSTP to the shell whenever the user types ctrl-z, catch it and suspend the foreground job by sending it a SIGTSTP

Test instructions:
make shell #compilie
./shell
cd
./hello_world $ #test for fg
fg 1 #move process to fg
./hello_world #test for bg
ctrl+z #suspend the foreground process
jobs #test for jobs
bg 1 #push the process to background and it will continue running
cat test.txt | gzip -c | gunzip -c | tail -n 10 #test for pipe
./hello_world
ctrl+c #stop the foreground process
exit

