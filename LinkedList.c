#include "headers.h"

// work on this command better based on the untokenized command

extern char completeCommandFore[4096];
extern char commandNameFore[32];

char *concatenateArgs(char *args[], int argc)
{
    // Calculate the total length of the concatenated string
    size_t totalLength = 0;
    for (int i = 0; i < argc; i++)
    {
        totalLength += strlen(args[i]);
        if (i < argc - 1)
        {
            // Add 1 for the space between arguments
            totalLength++;
        }
    }

    // Allocate memory for the concatenated string
    char *result = (char *)malloc(totalLength + 1); // +1 for the null terminator
    if (result == NULL)
    {
        perror("Memory allocation failed");
        exit(1);
    }

    // Concatenate the arguments with spaces
    result[0] = '\0'; // Initialize the result string
    for (int i = 0; i < argc; i++)
    {
        strcat(result, args[i]);
        if (i < argc - 1)
        {
            strcat(result, " "); // Add a space between arguments
        }
    }

    return result;
}

PtrToNode MakeNode(char *command[], pid_t currPID, int countArgs)
{
    PtrToNode P = (PtrToNode)malloc(sizeof(struct Node));
    if (command != NULL)
    {
        strcpy(P->commandName, command[0]);
        char *commandConcat = concatenateArgs(command, countArgs);
        strcpy(P->completeCommand, commandConcat);
    }
    else
    {
        strcpy(P->commandName, "Header");
        strcpy(P->completeCommand, "Header");
    }

    char proc_dir[50];
    snprintf(proc_dir, sizeof(proc_dir), "/proc/%d/status", currPID);

    // Use the stat system call to check if the directory exists
    // struct stat st;
    // if(stat(proc_dir, &st) != 0){
    //     printf("Process should be stopped here\n");
    //     temp->bgState=STOPPED;
    // }
    FILE *proc_status = fopen(proc_dir, "r");

    if (proc_status != NULL)
    {
        char line[256];
        while (fgets(line, sizeof(line), proc_status))
        {
            if (strstr(line, "State:") != NULL)
            {
                if (strstr(line, "T") != NULL)
                {
                    // Process is stopped
                    P->bgState = STOPPED;
                }
                else
                {
                    // Process is running
                    P->bgState = RUNNING;
                }
                break; // No need to continue reading the file
            }
        }
        fclose(proc_status);
    }

    P->pid = currPID;
    P->next = NULL;
    // free(commandConcat);

    return P;
}

List CreateEmptyList()
{
    PtrToNode P = MakeNode(NULL, -1, -1);
    return P;
}

void InsertAtFront(List L, char *X[], pid_t new, int countArgs)
{
    PtrToNode PNew = MakeNode(X, new, countArgs);

    PNew->next = L->next;
    L->next = PNew;
}

void DeleteNode(List L, PtrToNode P)
{
    // if(L!=NULL){
    //     PtrToNode temp=L->next;
    //     if(temp!=NULL){
    //         L->next=temp->next;
    //         temp->next=NULL;
    //         free(temp);
    //     }
    // }

    PtrToNode temp = L;

    while (temp->next != P)
    {
        temp = temp->next;
    }
    // have just reached behind P
    if (P != NULL)
    {
        temp->next = P->next;
        P->next = NULL;
        free(P);
    }
    else
    {
        return;
    }
}

void PrintList(List L)
{
    PtrToNode P = L->next; // Skip header
    // printf("\n");
    while (P != NULL)
    {
        printf("PID: %d, Command: %s \n", P->pid, P->completeCommand);
        P = P->next;
    }
}

void hasProcessTerminated(List L)
{
    PtrToNode temp = L->next;

    while (temp != NULL)
    {
        int status;
        pid_t child_result = waitpid(temp->pid, &status, WNOHANG);

        if (child_result > 0)
        {
            if (WIFEXITED(status))
            {
                if (!WEXITSTATUS(status))
                {
                    printf("%s exited normally (%d)\n", temp->commandName, temp->pid);
                }
                else
                {
                    printf("%s exited abruptly (%d)\n", temp->commandName, temp->pid);
                }
                DeleteNode(L, temp);
                temp = L; // Move to the next node
            }
            else if (WIFSIGNALED(status))
            {
                // printf("%s was killed by signal %d (%d)\n", temp->commandName, WTERMSIG(status), temp->pid);
                DeleteNode(L, temp);
                temp = L; // Move to the next node
            }
        }
        // process has not terminated yet
        temp = temp->next;
    }
}

int comparatorProc(const void *a, const void *b)
{
    struct Node *x = (struct Node *)a;
    struct Node *y = (struct Node *)b;

    return (x->pid) - (y->pid);
}

void activityCheck(List L)
{
    PtrToNode temp = L->next;
    hasProcessTerminated(L);
    // PrintList(L);
    struct Node *activeBackground = (struct Node *)malloc(sizeof(struct Node) * MAX_BG_PROC);
    int activeProc = 0;
    while (temp != NULL)
    {
        strcpy(activeBackground[activeProc].completeCommand, temp->completeCommand);
        strcpy(activeBackground[activeProc].commandName, temp->commandName);
        activeBackground[activeProc].pid = temp->pid;
        int status;
        // int procResult=waitpid(activeBackground[activeProc].pid, &status, WUNTRACED);       // WNOHANG is the other choice
        // if(procResult > 0)
        //     if (WIFSTOPPED(status)) {
        //         temp->bgState=STOPPED;
        //     }

        char proc_dir[50];
        snprintf(proc_dir, sizeof(proc_dir), "/proc/%d/status", temp->pid);

        // Use the stat system call to check if the directory exists
        // struct stat st;
        // if(stat(proc_dir, &st) != 0){
        //     printf("Process should be stopped here\n");
        //     temp->bgState=STOPPED;
        // }
        FILE *proc_status = fopen(proc_dir, "r");

        if (proc_status != NULL)
        {
            char line[256];
            while (fgets(line, sizeof(line), proc_status))
            {
                if (strstr(line, "State:") != NULL)
                {
                    if (strstr(line, "T") != NULL)
                    {
                        // Process is stopped
                        temp->bgState = STOPPED;
                    }
                    else
                    {
                        // Process is running
                        temp->bgState = RUNNING;
                    }
                    break; // No need to continue reading the file
                }
            }
            fclose(proc_status);
        }
        else
        {
            // Handle error opening the status file
            printf("Error opening the status file\n");
            return;
        }

        activeBackground[activeProc].bgState = temp->bgState;
        activeProc++;
        temp = temp->next;
    }
    qsort(activeBackground, activeProc, sizeof(struct Node), comparatorProc);
    for (int i = 0; i < activeProc; i++)
    {
        printf("%d : %s - %s\n", activeBackground[i].pid, activeBackground[i].completeCommand, activeBackground[i].bgState == RUNNING ? "Running" : "Stopped");
    }
    free(activeBackground);
}
