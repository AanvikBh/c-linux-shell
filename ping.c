#include "headers.h"

extern List backgroundlist;
extern List foregroundList;

extern pid_t currentForeground;
extern char executing_command;
extern pid_t terminal_pid;

extern char completeCommandFore[1024];
extern char commandNameFore[32];

int ping(int argCount, char *argString[])
{
    if (argCount <= 2)
    {
        printf("Usage format: ping <pid> <signal>\n");
        return 1;
    }
    pid_t target_pid = atoi(argString[1]);
    int firstCheck = kill(target_pid, 0);
    if (firstCheck == 0)
    {
        int sigVal = (atoi(argString[2])) % 32;
        int result = kill(target_pid, sigVal);
        if (result == 0)
        {
            printf("Sent signal %d to process with pid %d\n", sigVal, target_pid);
            // PrintList(backgroundlist);
            return 0;
        }
        else
        {
            perror("Error sending signal");
            return 1;
        }
    }
    else
    {
        printf("Process does not exist\n");
        return 1;
    }
}

void ctrlC_handler(int signum)
{
    // signal(SIGINT, SIG_IGN);
    // printf("%d\n", currentForeground);
    // printf("Ctrl C was clicked\n");

    // fflush(stdin);
    // fflush(stdout);
    // signal(SIGINT, SIG_IGN);
    printf("\n");
    if (currentForeground == 0)
    {
        // *****handle the empty case here*****
        // fputs("\n", stdin);
        // signal(SIGINT, SIG_DFL);
        return;
    }
    else
    {
        kill(currentForeground, SIGINT);
        currentForeground=0;
        // signal(SIGINT, SIG_DFL);
        return;
    }
    // kill(currentForeground,SIGINT);
    // return;
}

void ctrlZ_handler(int signum)
{
    printf("\n");
    //signal(SIGTSTP, SIG_IGN);
    if (currentForeground == 0)
    {
        //signal(SIGTSTP, SIG_DFL);
        return;
    }
    else
    {   
        // PrintList(foregroundList);

        PtrToNode P = (PtrToNode)malloc(sizeof(struct Node));
        strcpy(P->commandName, commandNameFore);
        strcpy(P->completeCommand, completeCommandFore);
        P->bgState = STOPPED;
        P->pid = currentForeground;
        if(backgroundlist->next!=NULL){
            P->next = backgroundlist->next->next;
        }
            
        else{
            P->next=NULL;
        }
            
        
        backgroundlist->next=P;

        int result = kill(currentForeground, SIGSTOP);

        if (result != 0)
        {
            perror("kill");
        }

        // *****************at one time only one stopped process is getting displayed*********************

        // char *foregroundCommand[1];
        // strcpy(foregroundCommand[0], foregroundList->next->commandName);
        
        // PrintList(foregroundList);
        DeleteNode(foregroundList, foregroundList->next);
        currentForeground=0;
        //signal(SIGTSTP, SIG_DFL);
        //PrintList(foregroundList);
        return;
    }
}

void ctrlD_handler(int signum)
{
    //signal(SIGQUIT, SIG_IGN);

    int result = kill(terminal_pid, SIGTERM);
    if(result != 0){
        perror("kill:");
    }
    // return;
    // PtrToNode temp = backgroundlist->next;

    // while (temp != NULL)
    // {
    //     int result;
    //     if ((result = kill(temp->pid, SIGKILL)) == -1)
    //     {
    //         perror("kill");
    //     }
    //     temp = temp->next;
    // }
    // char* args[]={"exit"};
    // execute_command(args, 1, 0, "exit");
    // // printf("\n");
    // printf("Ctrl D was clicked\n");
    // // kill(terminal_pid, SIGTSTP);
    // return;
    // signal(SIGQUIT, SIG_DFL);
    exit(EXIT_FAILURE);
    

}

