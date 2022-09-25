# osn_shell
Making a C shell for OSN course

Commands to run (after cd-ing to the directory containing the makefile) :

1 ) 
make

2 )
./shell

3 )
'exit' or 'quit' to exit the shell
Ctrl + D can also be used to exit the shell


Assumptions : 
1. In history, the most recent commands are displayed on top
2. In my 'discover' function, I have enabled search into hidden folders
3. When printing the time taken for execution of processes in the prompt, I display the total time taken by all the foreground processes that ran in that command.
4. The Virtual Memory allocated that is displayed in the 'piinfo' command is automatically displayed in the best units (kB, etc)
5. The output in 'discover' is not alphabetically sorted.
6. The directory from where the shell is launched will be set as the home (~) directory for the shell.
7. The paths should not contain any spaces.
8. Shell does not handle multiple inputs and outputs in the same command, but it can handle a single input and single output in the same command. (as was the requirement)
9. When a background process ends, a message is displayed. The cursor is moved to the next line, and the prompt is displayed.
10. main.c is the driver file for the shell.
11. parse.c contains the code for parsing the input command and sending it for execution.
12. All other files contain the code for the various commands that the shell supports. Their names are self-explanatory.