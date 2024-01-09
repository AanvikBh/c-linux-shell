#ifndef _PIPE_H_
#define _PIPE_H_

void pipe_create(char* argString[], int argCount, char* untokenized_str);

void splitArray(char input[], int inputSize, char **output, int *outputSize);

void pipe_redirect(char*** pipe_commands, int* pipe_sizes, int pipeCount, int currCommand, int this_pipe[],int prev_pipe[], int* file_desc,int standard_stdin,int standard_stdout);

#endif