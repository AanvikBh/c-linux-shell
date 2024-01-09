#include "headers.h"

extern List backgroundlist;

extern pid_t terminal_pid;

volatile int bgSwitchSig = 0;

void switch_bg_to_fg(int signum)
{
    bgSwitchSig = 1;
}

void bg(char *argString[], int argCount)
{
    if (argCount < 2)
    {
        printf("Incorrect usage of bg, pid missing.\n");
        return;
    }
    pid_t reqProc = atoi(argString[1]);

    PtrToNode temp = backgroundlist->next;

    while (temp != NULL)
    {
        if (temp->pid == reqProc)
        {
            int s_to_r = kill(temp->pid, SIGCONT);
            if (s_to_r == -1)
            {
                perror("Error: ");
                return;
            }
            return;
        }
        temp = temp->next;
    }
    printf("No background process with the pid exists\n");
    return;
}

void fg(char *argString[], int argCount)
{
    PtrToNode temp = backgroundlist->next;
    while (temp != NULL)
    {
        if (temp->pid == atoi(argString[1]))
        {
            // bring this process to foreground
            // kill(terminal_pid, SIGUSR1);
            
            // Bring the process to the foreground
            tcsetpgrp(STDIN_FILENO, getpgid(temp->pid));
            kill(temp->pid, SIGCONT);
            int status;
            // Wait for the process to finish or be stopped
            waitpid(temp->pid, &status, WUNTRACED);

            // Return control to the shell
            tcsetpgrp(STDIN_FILENO, getpgrp());
            DeleteNode(backgroundlist, temp);
            return;
        }
        temp=temp->next;
    }
    printf("No process found\n");
    return;
}