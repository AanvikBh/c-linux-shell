# Description:

This is a C based terminal with basic implementation of functions. Any function not specified to be defined individually is handled by execvp(). The '~' in prompt represents home directory of the shell which may not be the home directory of the terminal on which the shell is running. 
All the necessary header files for all the function specifications are included in "headers.h" and then all personally defined function definitions are in "sepcification.h" where specification is the name of the function implemented. 
To start the terminal, enter make in the directory which contains the makefile and all the other files mentioned here, and then run the executable a.out.

# Assumptions:

warp:
If the user is at the home directory from which shell was invoked and they do "warp .." then they will be directed behind the home directory, and absolute path will be displayed. If the current directory is a superset of home directory then relative path will be displayed in the prompt.
If a path like "~/some/path" is provided, ~ will be considered as shell's directory and not the terminal's home directory.
Another assumption, as specified in the mini project, is that the absolute path should not contain any spaces in the directory name.

peek:
Implementation of "peek -" will not be valid and may not give the desired effects since we are not actually changing the directories here. So it will print information for current directory.
Flags like "-alalaaa" will not be considered equivalent to "-a -l" and if there are any other characters other than 'a' or 'l' in the flag then it will be considered invalid. 
If a path like "~/some/path" is provided, ~ will be considered as shell's directory and not the terminal's home directory.

seek:
In some cases it will give extra error flags, which are yet to be fixed. But the overall working is still fine.

pastevents:
Currently commands like "sleep 2; pastevents execute 1" will disturb the whole "pastevents.txt" file, so it is not accounted for in this submission. Otherwise, commands are getting parsed in "pastevents.txt".
"pastevents.txt" will stay in the home directory of shell, so we are using an absolute path to access it to read/write to it. 
There is some malicious behaviour happening for commands which are separated by ; delimiter, it leads to removal of the whole history for some reason, the approach will be changed later. 

tokenize:
First tokenization is done base on ';', followed by '&' and '\t'. Any other delimiters will not be tokenized in this case. 

exit:
If you want to terminate the shell, then you have to enter exit command to send the break_signal (only for the sake of Part A). 

linkedlist:
Linked list is used to store the pid of background processes, and after entering a prompt it is completely traversed to see if the node containing a given pid is still terminated or not. 

neonate:
The input is assumed to be an integer, and in case a float is entered, sleep will still treat it as a valid time and neonate will work as intended.


activities:
We are only dealing with background processes in this command. All the commands are stored in the same data structure as used in storing process info of backgrounf processes.

ping:


signals:
Ctrl C- After the shell is spawned and it is clicked/when no foreground process is happening, then it will be in a blank state, and after clicking enter, prompt will re-appear.
Ctrl D- Due to some reasons, it is preferred to avoid clicking this command since it will crash the shell. It has been implemented in the form of a command called "exit" as mentioned above.
Ctrl Z- Name of the process is not appearing, but the transition from foreground to background is pretty much visible. This can be worked upon later.

arrow redirection:
The redirection tokens (< , >, >>) should be separated by a space, since in the code it is being treated as a single token, and tokenization has not been done on the basis of it. 

pipe:
It is not working for some user-defined commands, may work for some execvp based commands. Also they should be space separated liek arrow redirection.

Note: Multiple error messages might get printed if any error is encountered due to ease in understanding of the source of error. This will be fixed later.
