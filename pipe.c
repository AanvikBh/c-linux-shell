#include "headers.h"

extern int standard_stdin;
extern int standard_stdout;

extern char untoken[4096];
extern char homeDirectory[max_path];

void splitArray(char input[], int inputSize, char **output, int *outputSize) {
    int subArraySize = 0;
    int subArrayCount = 0;
    int inSubArray = 0;

    // Count the number of sub-arrays (arguments separated by pipes)
    for (int i = 0; i < inputSize; i++) {
        if (strcmp(input[i], "|") == 0) {
            if (inSubArray) {
                // End of a sub-array
                inSubArray = 0;
                output[subArrayCount][subArraySize] = NULL; // Add a null character
                outputSize[subArrayCount] = subArraySize;
                subArrayCount++;
            }
        } else {
            if (!inSubArray) {
                // Start of a new sub-array
                inSubArray = 1;
                subArraySize = 0;
            }
            output[subArrayCount][subArraySize] = strdup(input[i]); // Allocate memory and copy the string
            subArraySize++;
        }
    }

    // Add a null character to the last sub-array (if it exists)
    if (inSubArray) {
        output[subArrayCount][subArraySize] = NULL;
        outputSize[subArrayCount] = subArraySize;
    }

    // Set the size of the output array
    outputSize[subArrayCount + 1] = -1;
}


// void pipe_create(char* argString[], int argCount, char* untokenized_str){
//     int pipe_count=0;
//     for(int i=0; i<argCount; i++){
//         if(strcmp(argString[i], "|")==0)    pipe_count++;
//     }
//     int pipe_seen=0;
//     char **pipe_separated_commands[20];  // Assuming a maximum of 10 sub-arrays
//     int pipe_separated_size[20]; // Extra element for -1 to mark the end
//     // Initialize outputSize
//     for (int i = 0; i < 20; i++) {
//         pipe_separated_size[i] = 0;
//     }
//     // Allocate memory for sub-arrays and call the function to split the input
//     for (int i = 0; i < 20; i++) {
//         pipe_separated_commands[i] = (char **)malloc(sizeof(char *) * 32); // Assuming the same size as input
//     }
//     splitArray(argString, argCount, pipe_separated_commands, pipe_separated_size);
//     int prev_pipe[2];
//     int curr_pipe[2];
//     if (pipe(prev_pipe) == -1 || pipe(curr_pipe) == -1) {
//         perror("pipe");
//         exit(EXIT_FAILURE);
//     }
//     int pipe_input_fd = prev_pipe[1];
//     int pipe_output_fd = curr_pipe[0];
//     for(pipe_seen=0; pipe_seen<=pipe_count; pipe_seen++){
//         if(pipe_seen==pipe_count){
//             // at the last cell, so you just have to dup stdout at curr[1]
//             close(curr_pipe[0]);
//             if(dup2(curr_pipe[1], STDOUT_FILENO)==-1){
//                 perror("Dup2:");
//                 exit(1);
//             }
//             execute_command(pipe_separated_commands[pipe_seen], pipe_separated_size[pipe_seen], 0, untokenized_str);
//             reset_fd();
//         }
//         else if(pipe_seen==0){
//             // at the first cell, so you just have to dupp stdin at prev[0], and redirect its output to curr[0]
//             if(dup2(prev_pipe[0], STDIN_FILENO)==-1){
//                 perror("Dup2:");
//                 exit(1);
//             }
//             if(dup2(prev_pipe[1], curr_pipe[0])==-1){
//                 perror("Dup2:");
//                 exit(1);
//             }
//             close(prev_pipe[0]);
//             close(prev_pipe[1]);
//             execute_command(pipe_separated_commands[pipe_seen], pipe_separated_size[pipe_seen], 0, untokenized_str);
//         }
//         else{
//             //taking input from curr pipe to prev pipe
//             if(dup2(prev_pipe[0], curr_pipe[1])==-1){
//                 perror("Dup2:");
//                 exit(1);
//             }
//             close(prev_pipe[1]);
//             close(curr_pipe[0]);
//             if(dup2(curr_pipe[0], prev_pipe[1])==-1){
//                 perror("Dup2:");
//                 exit(1);
//             }
//             close(prev_pipe[0]);
//             close(curr_pipe[1]);
//             execute_command(pipe_separated_commands[pipe_seen], pipe_separated_size[pipe_seen], 0, untokenized_str);
//         }
//     }
//}

void pipe_create(char* argString[], int argCount, char* untokenized_str){
    int pipe_count=0;
    for(int i=0; i<argCount; i++){
        if(strcmp(argString[i], "|")==0)    pipe_count++;
    }
    

    char **pipe_separated_commands[20];  // Assuming a maximum of 10 sub-arrays
    int pipe_separated_size[20]; // Extra element for -1 to mark the end

    // Initialize outputSize
    for (int i = 0; i < 20; i++) {
        pipe_separated_size[i] = 0;
    }

    // Allocate memory for sub-arrays and call the function to split the input
    for (int i = 0; i < 20; i++) {
        pipe_separated_commands[i] = (char **)malloc(sizeof(char *) * 32); // Assuming the same size as input
    }

    splitArray(argString, argCount, pipe_separated_commands, pipe_separated_size);

    int pipe_seen;
    int pipe_curr[2];
    int pipe_prev[2];
    if(pipe(pipe_curr)==-1){
        perror("pipe:");
    }
    int pipe_io=STDIN_FILENO;
    int standard_stdin,standard_stdout;
    standard_stdin=dup(STDIN_FILENO);
    standard_stdout=dup(STDOUT_FILENO);
    for(pipe_seen=0; pipe_seen<=pipe_count; pipe_seen++){
        pipe_redirect(pipe_separated_commands, pipe_separated_size, pipe_count, pipe_seen, pipe_curr,pipe_prev, &pipe_io,standard_stdin,standard_stdout);
    }
    dup2(standard_stdin,STDIN_FILENO);
    close(pipe_curr[0]);
    close(pipe_curr[1]);
    reset_fd();
}


void pipe_redirect(char*** pipe_commands, int* pipe_sizes, int pipeCount, int currCommand, int this_pipe[],int prev_pipe[], int* file_desc,int standard_stdin,int standard_stdout){
    // pid_t pid = fork();

    
        
        // Child process
        // char** pipe_commands[currCommand]=redirect_driver(argString, argCount, untokenized_str);
        // if(pipe(this_pipe)==-1){
        //     perror("pipe:");
        // }
        // close(this_pipe[0]);
        if(currCommand==0){
            *file_desc=0;
            dup2(this_pipe[1], STDOUT_FILENO);
            close(this_pipe[1]);
            // printf("h\n");


        prev_pipe[0]=this_pipe[0];
        prev_pipe[1]=this_pipe[1];
            

        }
        else if(currCommand==pipeCount){    // pipeCount +-1    ??? ****
            dup2(this_pipe[0], STDIN_FILENO);

            dup2(standard_stdout,STDOUT_FILENO);
        }
        else{
            
            dup2(prev_pipe[0], STDIN_FILENO);
            dup2(this_pipe[1], STDOUT_FILENO);

        prev_pipe[0]=this_pipe[0];
        prev_pipe[1]=this_pipe[1];

            
        }
        close(this_pipe[1]);
        
        // need to add redirection for user defined commands
        // printf("%s\n", pipe_commands[currCommand][0]);
        // printf("%d\n", pipe_sizes[currCommand]);

        if(strcmp(pipe_commands[currCommand][0], "warp")==0){
            int warpStat=warp(pipe_commands[currCommand], pipe_sizes[currCommand], homeDirectory);
            if(warpStat==-1){
                printf("Warp encountered some error\n");
            }
            reset_fd();
            exit(EXIT_SUCCESS);
        }

        if(strcmp(pipe_commands[currCommand][0], "peek")==0){
            int peekStat=peek(pipe_sizes[currCommand], pipe_commands[currCommand]);
            if(peekStat==1){
                printf("Peek failed: Ensure that the proper command is inputted\n");
            }
            reset_fd();
            exit(EXIT_SUCCESS);
        }

        if(strcmp(pipe_commands[currCommand][0], "proclore")==0){
            proclore(pipe_sizes[currCommand], pipe_commands[currCommand]);
            reset_fd();
            exit(EXIT_SUCCESS);
        }

        if(strcmp(pipe_commands[currCommand][0], "pastevents")==0){
            // int pasteventStat=pastevents(pipe_sizes[currCommand], pipe_commands[currCommand], originalStr);
            // if(pasteventStat==1){
            //     printf("Pastevents failed: Ensure that the proper comand in inputted\n");
            // }
            // return;
            int pasteventStat=pastevents(pipe_sizes[currCommand], pipe_commands[currCommand]);
            if(pasteventStat!=0){
                printf("Pastevents execution failed\n");
            }
            reset_fd();
            exit(EXIT_SUCCESS);
        }

        if(strcmp(pipe_commands[currCommand][0], "seek")==0){
            int seekStat=seek(pipe_sizes[currCommand], pipe_commands[currCommand]);
            if(seekStat==1){
                printf("Seek failed: Ensure that the proper command is inputted\n");
            }
            reset_fd();
            exit(EXIT_SUCCESS);
        }

        if(strcmp(pipe_commands[currCommand][0], "ping")==0){
            int pingStat=ping(pipe_sizes[currCommand], pipe_commands[currCommand]);
            if(pingStat==1){
                printf("Ping failed: Signal has not been sent.\n");
            }
            reset_fd();
            exit(EXIT_SUCCESS);
        }

        if(strcmp(pipe_commands[currCommand][0], "activities")==0){
            int actStat=activities(pipe_commands[currCommand], pipe_sizes[currCommand]);
            if(actStat==1){
                printf("Activities failed: Check the input format or .\n");
            }
            reset_fd();
            exit(EXIT_SUCCESS);
        }

        if(strcmp(pipe_commands[currCommand][0], "fg")==0){
            fg(pipe_commands[currCommand], pipe_sizes[currCommand]);
            reset_fd();
            exit(EXIT_SUCCESS);
        }

        if(strcmp(pipe_commands[currCommand][0], "bg")==0){
            bg(pipe_commands[currCommand], pipe_sizes[currCommand]);
            reset_fd();
            exit(EXIT_SUCCESS);
        }

        if(strcmp(pipe_commands[currCommand][0], "iMan")==0){
            int iManStat=iMan(pipe_commands[currCommand], pipe_sizes[currCommand], 80);
            if(iManStat!=0){
                reset_fd();
                perror("Error:");
                exit(EXIT_FAILURE);
            }
            reset_fd();
            exit(EXIT_SUCCESS);
        }
        
        if(strcmp(pipe_commands[currCommand][0], "neonate")==0){
            int neoStat=neonate(pipe_sizes[currCommand], pipe_commands[currCommand]);
            if(neoStat!=0){
                printf("Neonate gave some error\n");
                reset_fd();
                exit(EXIT_FAILURE);
            }
            reset_fd();
            exit(EXIT_SUCCESS);
        }

        else{
            // not user defined commands
            // printf("%d\n",*file_desc);
            // printf("here %s\n", pipe_commands[currCommand][0]);
            int pid=fork();
            if(pid==0){
            execvp(pipe_commands[currCommand][0], pipe_commands[currCommand]);
            perror("Failed to execute command");
            exit(EXIT_FAILURE);}
            else if(pid>0){
                wait(NULL);
            }
        }
        
    
    
        // Parent process
        // time_t start_time = time(NULL);
        if(*(file_desc)!=STDIN_FILENO){
            close(*file_desc);
        }
        close(this_pipe[1]);
        int status;
        //printf("stdin %d stdout %d stdoutapp %d\n", stdin_redirection, stdout_redirection, stdout_redirection_append);
        
        
        
        //printf("child returned after wait 1\n");
        // if (WIFEXITED(status))
        // {
        //     time_t end_time = time(NULL);
        //     double time_taken = difftime(end_time, start_time);
        //     // if (time_taken > 2)
        //     // {
        //     //     printf("%s took %d seconds to terminate.\n", argString[0], (int)time_taken);
        //     // }
        // }
    
    return;
}


// void pipe_create(char *argString[], int argCount, char *untokenized_str) {
//     int pipe_count = 0;
//     for (int i = 0; i < argCount; i++) {
//         if (strcmp(argString[i], "|") == 0) pipe_count++;
//     }
//     char **pipe_separated_commands[20];  // Assuming a maximum of 20 sub-commands
//     int pipe_separated_size[20]; // Extra element for -1 to mark the end
//     // Initialize outputSize
//     for (int i = 0; i < 20; i++) {
//         pipe_separated_size[i] = 0;
//     }
//     // Allocate memory for sub-commands and call the function to split the input
//     for (int i = 0; i < 20; i++) {
//         pipe_separated_commands[i] = (char **)malloc(sizeof(char *) * 32); // Assuming the same size as input
//     }
//     splitArray(argString, argCount, pipe_separated_commands, pipe_separated_size);
//     int prev_pipe[2];
//     int curr_pipe[2];
//     if (pipe(prev_pipe) == -1 || pipe(curr_pipe) == -1) {
//         perror("pipe");
//         exit(1);
//     }
//     int input_fd = standard_stdin; // Initialize input_fd to standard input
//     int output_fd = prev_pipe[1];  // Initialize output_fd to the write end of the previous pipe
//     for (int pipe_seen = 0; pipe_seen <= pipe_count; pipe_seen++) {
//         if (pipe_seen == pipe_count) {
//             // Last sub-command, set output_fd to standard output
//             output_fd = standard_stdout;
//         }
//         execute_command(pipe_separated_commands[pipe_seen], pipe_separated_size[pipe_seen], 0, untoken);
//         if (pipe_seen != pipe_count) {
//             // Close the pipe's read and write ends after using them
//             close(prev_pipe[0]);
//             close(prev_pipe[1]);    
//             // Swap pipes for the next iteration
//             int *temp = prev_pipe;
//             prev_pipe = curr_pipe;
//             curr_pipe = temp;    
//             // Set input_fd for the next sub-command to the read end of the current pipe
//             input_fd = curr_pipe[0];
//         }
//     }
//     // Free allocated memory for sub-commands
//     for (int i = 0; i < 20; i++) {
//         free(pipe_separated_commands[i]);
//     }
// }