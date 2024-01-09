#include "headers.h"

extern int break_signal;

char completeCommandFore[1024];
char commandNameFore[32];

extern List backgroundlist;
extern List activitiesList;
extern List foregroundList;

extern char executing_command[32];

extern char homeDirectory[max_path];

extern char untoken[4096];

extern pid_t currentForeground;

extern int standard_stdin;
extern int standard_stdout;

extern pid_t child_parent_pid;

void execute_command(char *args[], int countArgs, int background, char* originalStr) {
    int redirection_present=0;
    int pipe_present=0;
    for(int i=0; i<countArgs; i++){
        if(strcmp("<", args[i])==0 || strcmp(">", args[i])==0 || strcmp(">>", args[i])==0){
            adding_pastevents(countArgs, args);
            redirection_present=1;
            redirect(args, countArgs, originalStr);
            return;
        }
        else if(strcmp("|", args[i])==0){
            adding_pastevents(countArgs, args);
            pipe_present=1;
            pipe_create(args, countArgs, originalStr);
            return;
        }
    }

    strcpy(executing_command, args[0]);

    if(!redirection_present || !pipe_present){
        adding_pastevents(countArgs, args);
    }

        if(strcmp(args[0], "warp")==0){
            int warpStat=warp(args, countArgs, homeDirectory);
            reset_fd();
            if(warpStat==-1){
                printf("Warp encountered some error\n");
            }
            return;
        }

        if(strcmp(args[0], "peek")==0){
            int peekStat=peek(countArgs, args);
            reset_fd();
            if(peekStat==1){
                printf("Peek failed: Ensure that the proper command is inputted\n");
            }
            return;
        }

        if(strcmp(args[0], "proclore")==0){
            proclore(countArgs, args);
            reset_fd();
            return;
        }

        if(strcmp(args[0], "pastevents")==0){
            // int pasteventStat=pastevents(countArgs, args, originalStr);
            // if(pasteventStat==1){
            //     printf("Pastevents failed: Ensure that the proper comand in inputted\n");
            // }
            // return;
            int pasteventStat=pastevents(countArgs, args);
            reset_fd();
            if(pasteventStat!=0){
                printf("Pastevents execution failed\n");
            }
            return;
        }

        if(strcmp(args[0], "seek")==0){
            int seekStat=seek(countArgs, args);
            reset_fd();
            if(seekStat==1){
                printf("Seek failed: Ensure that the proper command is inputted\n");
            }
            return;
        }

        if(strcmp(args[0], "exit")==0){
            break_signal=1;
            exit(EXIT_SUCCESS);

        }

        if(strcmp(args[0], "ping")==0){
            int pingStat=ping(countArgs, args);
            reset_fd();
            if(pingStat==1){
                printf("Ping failed: Signal has not been sent.\n");
            }
            return;
        }

        if(strcmp(args[0], "activities")==0){
            int actStat=activities(args, countArgs);
            reset_fd();
            if(actStat==1){
                printf("Activities failed: Check the input format or .\n");
            }
            return;
        }

        if(strcmp(args[0], "fg")==0){
            fg(args, countArgs);
            return;
        }

        if(strcmp(args[0], "bg")==0){
            bg(args, countArgs);
            return;
        }

        if(strcmp(args[0], "iMan")==0){
            int iManStat=iMan(args, countArgs, 80);
            reset_fd();
            if(iManStat!=0){
                perror("Error:");
                return;
            }
            return;
        }
        
        if(strcmp(args[0], "neonate")==0){
            int neoStat=neonate(countArgs, args);
            reset_fd();
            if(neoStat!=0){
                printf("Neonate gave some error\n");
                return;
            }
            return;
        }

        if(!background){
            pid_t pid = fork();

            if (pid < 0) {
                perror("Fork failed");
            } else if (pid == 0) {
                // Child process
                InsertAtFront(foregroundList, args, child_parent_pid, countArgs);
                strcpy(commandNameFore, args[0]);
                char* tempPtr=concatenateArgs(args, countArgs);
                
                strcpy(completeCommandFore, tempPtr);
                //printf("%s\n", completeCommandFore);
                //PrintList(foregroundList);
                //printf("printing inside child\n");
                if (execvp(args[0], args) == -1) {
                    DeleteNode(foregroundList, foregroundList->next);
                    perror("Execution failed");
                    exit(EXIT_FAILURE);             //changing exit(FAILURE)
                }
            } else {
                // Parent process
                // tcsetpgrp(0, pid);
                currentForeground=pid;
                child_parent_pid=pid;
                time_t start_time = time(NULL);
                int status;
                
                waitpid(pid, &status, WUNTRACED);  // Wait for the child to finish
                // tcsetpgrp(0, getpgid(0));
                //printf("printing inside parent\n");
                //PrintList(foregroundList);
                //DeleteNode(foregroundList, foregroundList->next);
                if (WIFEXITED(status)) {
                    
                    time_t end_time = time(NULL);
                    double time_taken = difftime(end_time, start_time);
                    if(time_taken>2){
                        printf("%s took %d seconds to terminate.\n",executing_command, (int)time_taken);
                    }

                }
            }
        }
        else {
            pid_t pid_bg = fork();
            if (pid_bg < 0) {
                perror("Fork failed");
            } else if (pid_bg == 0) {
                // Child process
                if (execvp(args[0], args) == -1) {
                    perror("Execution failed");
                    exit(EXIT_FAILURE);
                }
            } else {
                // Parent process
                //store pid and name, fgets for next call check ll and check whether the status of project has been completed or not
                printf("%d\n", pid_bg);
                InsertAtFront(backgroundlist, args, pid_bg, countArgs);
                
                // printf("Background process started with PID: %d\n", pid_bg);
            }
        }
}
