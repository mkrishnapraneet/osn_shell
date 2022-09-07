# osn_shell
Making a C shell for OSN course

Commands to run (after cd-ing to the directory containing the makefile) :

1 ) 
make

2 )
./shell



Assumptions : 
1. In history, the most recent commands are displayed on top
2. In my 'discover' function, I have enabled search into hidden folders
3. When printing the time taken for execution of processes in the prompt, I display the total time taken by all the foreground processes that ran in that command.
4. The Virtual Memory allocated that is displayed in the 'piinfo' command is automatically displayed in the best units (kB, etc)
5. The output in 'discover' is not alphabetically sorted.
6. The directory from where the shell is launched will be set as the home (~) directory for the shell.
7. The paths should not contain any spaces.