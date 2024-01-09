#include "headers.h"

int token_idx = 0;
int stdin_redirection = 0;
int stdout_redirection = 0;
int stdout_redirection_append = 0;
int input_fd = -1;
int output_fd = -1;
int tempGlobal;

extern int standard_stdin;
extern int standard_stdout;

extern char input_file[max_name_len];
extern char output_file[max_name_len];
extern char output_append_file[max_name_len];

extern char homeDirectory[max_path];


void reset_fd(){
    if (dup2(standard_stdout, STDOUT_FILENO) == -1)
    {
        perror("Failed to redirect stdout");
        exit(EXIT_FAILURE);
    }

    if (dup2(standard_stdin, STDIN_FILENO) == -1)
    {
        perror("Failed to redirect stdout");
        exit(EXIT_FAILURE);
    }
    return;

}

char** redirect_driver(char *newArgs[], int argCount, char *untokenized_str)
{
    //printf("Redirected\n");
    for (int i = 0; i < argCount; i++)
    {
        if (strcmp(newArgs[i], "<") == 0)
        {
            // redirect input from stdin to the txt file given
            //printf("Input redirection\n");
            // token_idx = i;
            stdin_redirection = 1;
            strcpy(input_file, newArgs[i+1]);
        }
        else if (strcmp(newArgs[i], ">") == 0)
        {
            // redirect output from stdout to the txt file given
            // printf("Output redirection\n");
            stdout_redirection = 1;
            strcpy(output_file, newArgs[i+1]);
        }
        else if (strcmp(newArgs[i], ">>") == 0)
        {
            // redirect output from stdout to the txt file given(append)
            stdout_redirection_append = 1;
            strcpy(output_append_file, newArgs[i+1]);
        }
        else if (strcmp(newArgs[i], "<") == 0 && i + 2 <= argCount && strcmp(newArgs[i + 2], ">>") == 0)
        {
            // redirect input and output both+append
            stdin_redirection=1;
            stdout_redirection_append=1;
            strcpy(input_file, newArgs[i+1]);
            strcpy(output_append_file, newArgs[i+3]);
        }
        else if (strcmp(newArgs[i], "<") == 0 && i + 2 <= argCount && strcmp(newArgs[i + 2], ">") == 0)
        {
            // redirect input and output both
            stdin_redirection=1;
            stdout_redirection=1;
            strcpy(input_file, newArgs[i+1]);
            strcpy(output_file, newArgs[i+3]);
        }
    }
    // Now, execute the command (e.g., newArgs[0]) with the specified redirections.
    // execvp(newArgs[0], newArgs);


    //printf("Execution time\n");
    char **execArgs=(char**)malloc(sizeof(char*)*(argCount + 1));  // +1 for the NULL terminator
    int execArgCount = 0;
    for(int i=0; i<=argCount; i++){
        execArgs[i]=(char*)malloc(sizeof(char)*max_name_len);
    }

    // Flag to skip the argument immediately following '>'
    int skipNextArg = 0;

    // Copy only the command and its arguments to the new array
    for (int i = 0; i < argCount; i++) {
        if (skipNextArg) {
            // Skip the argument immediately following '>'
            skipNextArg = 0;
            continue;
        }

        if (strcmp(newArgs[i], "<") == 0 || strcmp(newArgs[i], ">") == 0 || strcmp(newArgs[i], ">>") == 0) {
            // Skip redirection tokens and set the flag to skip the next argument
            skipNextArg = 1;
            continue;
        }
        
        execArgs[execArgCount] = newArgs[i];
        execArgCount++;
    }
    execArgs[execArgCount] = NULL;  // Null-terminate the new array
    tempGlobal=execArgCount;
    // execlp(execArgs[0], execArgs[0], (char *)NULL);
    // execvp(execArgs[0], execArgs);
    // perror("Failed to execute command");
    // exit(1);
    //printf("stdin %d stdout %d stdoutapp %d\n", stdin_redirection, stdout_redirection, stdout_redirection_append);
    return execArgs;
}



int redirect(char *argString[], int argCount, char *untokenized_str){
    pid_t pid = fork();

    if (pid < 0)
    {
        perror("Fork failed");
    }
    else if (pid == 0)
    {
        standard_stdin=dup(STDIN_FILENO);
        standard_stdout=dup(STDOUT_FILENO);
        // Child process
        char** newArgs=redirect_driver(argString, argCount, untokenized_str);
        
        if(stdin_redirection){
                //printf("Input here 2\n");
                input_fd = open(input_file, O_RDONLY);
                if (input_fd == -1)
                {
                    perror("No such input file found!");
                    exit(1);
                }
                // Redirect stdin to the input file using dup2
                if (dup2(input_fd, STDIN_FILENO) == -1)
                {
                    perror("Failed to redirect stdin");
                    exit(1);
                }
                close(input_fd);
        }

        if (stdout_redirection) {

                //printf("output here 2\n");
                output_fd = open(output_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
                if (output_fd == -1)
                {
                    perror("Failed to open or create output file");
                    exit(1);
                }
                // Redirect stdout to the output file using dup2
                if (dup2(output_fd, STDOUT_FILENO) == -1)
                {
                    perror("Failed to redirect stdout");
                    exit(1);
                }
                close(output_fd);
        }
        if(stdout_redirection_append){
                //printf("output append here 2\n");
                output_fd = open(output_append_file, O_WRONLY | O_CREAT | O_APPEND, 0644);
                if (output_fd == -1)
                {
                    perror("Failed to open or create output file");
                    exit(1);
                }
                // Redirect stdout to the output file using dup2
                if (dup2(output_fd, STDOUT_FILENO) == -1)
                {
                    perror("Failed to redirect stdout");
                    exit(1);
                }
                close(output_fd);
        }
        //printf("exec time 2\n");
        stdin_redirection=0;
        stdout_redirection=0;
        stdout_redirection_append=0;


        // need to add redirection for user defined commands
        // printf("%s\n", newArgs[0]);
        // printf("%d\n", tempGlobal);

        if(strcmp(newArgs[0], "warp")==0){
            int warpStat=warp(newArgs, tempGlobal, homeDirectory);
            if(warpStat==-1){
                printf("Warp encountered some error\n");
            }
            reset_fd();
            exit(EXIT_SUCCESS);
        }

        if(strcmp(newArgs[0], "peek")==0){
            int peekStat=peek(tempGlobal, newArgs);
            if(peekStat==1){
                printf("Peek failed: Ensure that the proper command is inputted\n");
            }
            reset_fd();
            exit(EXIT_SUCCESS);
        }

        if(strcmp(newArgs[0], "proclore")==0){
            proclore(tempGlobal, newArgs);
            reset_fd();
            exit(EXIT_SUCCESS);
        }

        if(strcmp(newArgs[0], "pastevents")==0){
            // int pasteventStat=pastevents(tempGlobal, newArgs, originalStr);
            // if(pasteventStat==1){
            //     printf("Pastevents failed: Ensure that the proper comand in inputted\n");
            // }
            // return;
            int pasteventStat=pastevents(tempGlobal, newArgs);
            if(pasteventStat!=0){
                printf("Pastevents execution failed\n");
            }
            reset_fd();
            exit(EXIT_SUCCESS);
        }

        if(strcmp(newArgs[0], "seek")==0){
            int seekStat=seek(tempGlobal, newArgs);
            if(seekStat==1){
                printf("Seek failed: Ensure that the proper command is inputted\n");
            }
            reset_fd();
            exit(EXIT_SUCCESS);
        }

        if(strcmp(newArgs[0], "ping")==0){
            int pingStat=ping(tempGlobal, newArgs);
            if(pingStat==1){
                printf("Ping failed: Signal has not been sent.\n");
            }
            reset_fd();
            exit(EXIT_SUCCESS);
        }

        if(strcmp(newArgs[0], "activities")==0){
            int actStat=activities(newArgs, tempGlobal);
            if(actStat==1){
                printf("Activities failed: Check the input format or .\n");
            }
            reset_fd();
            exit(EXIT_SUCCESS);
        }

        if(strcmp(newArgs[0], "fg")==0){
            fg(newArgs, tempGlobal);
            reset_fd();
            exit(EXIT_SUCCESS);
        }

        if(strcmp(newArgs[0], "bg")==0){
            bg(newArgs, tempGlobal);
            reset_fd();
            exit(EXIT_SUCCESS);
        }

        if(strcmp(newArgs[0], "iMan")==0){
            int iManStat=iMan(newArgs, tempGlobal, 80);
            if(iManStat!=0){
                reset_fd();
                perror("Error:");
                return 1;
            }
            reset_fd();
            exit(EXIT_SUCCESS);
        }
        
        if(strcmp(newArgs[0], "neonate")==0){
            int neoStat=neonate(tempGlobal, newArgs);
            if(neoStat!=0){
                printf("Neonate gave some error\n");
                reset_fd();
                return 1;
            }
            reset_fd();
            exit(EXIT_SUCCESS);
        }

        else{
            // not user defined commands
            execvp(newArgs[0], newArgs);
            perror("Failed to execute command");
            exit(EXIT_FAILURE);
        }
    }
    else
    {
        // Parent process
        time_t start_time = time(NULL);
        int status;
        //printf("stdin %d stdout %d stdoutapp %d\n", stdin_redirection, stdout_redirection, stdout_redirection_append);
        waitpid(pid, &status, 0); // Wait for the child to finish
        //printf("child returned after wait 1\n");
        if (WIFEXITED(status))
        {
            time_t end_time = time(NULL);
            double time_taken = difftime(end_time, start_time);
            if (time_taken > 2)
            {
                printf("%s took %d seconds to terminate.\n", argString[0], (int)time_taken);
            }
        }
    }
    return 0;
}

