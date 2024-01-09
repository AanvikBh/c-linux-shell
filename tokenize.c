#include "headers.h"

extern bool pastevents_or_purge;

extern char untoken[4096];

char *strtrim(char *str) {
    while (isspace((unsigned char)*str)) {
        str++;
    }
    if (*str == 0) {
        return str;
    }
    char *end = str + strlen(str) - 1;
    while (end > str && isspace((unsigned char)*end)) {
        end--;
    }
    *(end + 1) = '\0';
    return str;
}


void tokenize (char* input) {
    input[strcspn(input, "\n")] = '\0';

        // Tokenize input based on ';'
        char *commands[MAX_COMMANDS];
        int command_count = 0;

        char *command = strtok(input, ";");
        while (command != NULL) {
            commands[command_count++] = strtrim(command);  // Trim whitespace from command
            //printf("%s\n", commands[command_count-1]);
            command = strtok(NULL, ";");
        }
        //printf("%d\n", command_count);
        int backgroundProcess[MAX_COMMANDS];
        for (int i = 0; i < MAX_COMMANDS; i++){
            backgroundProcess[i] = 0;
        }
        // Tokenize and execute commands
        for (int i = 0; i < command_count; i++) {
            char *subcommands[MAX_COMMANDS];
            int subcommand_count = 0;

            int background_count=0;
            for(int j=0; j<strlen(commands[i]); j++){
                if(commands[i][j] == '&'){
                    background_count++;
                }
            }

            for (int k = 0; k < background_count; k++){
                backgroundProcess[k] = 1;
            }
            // Tokenize command based on '&'
            char *subcommand = strtok(commands[i], "&");
            while (subcommand != NULL) {
                subcommands[subcommand_count++] = strtrim(subcommand);  // Trim whitespace from subcommand
                subcommand = strtok(NULL, "&");
            }
            
            // printf("%d\n", subcommand_count);
            // for(int i=0; i<subcommand_count-1; i++){
            //     // all processes tokenized by & should be in background except the very last one
                
            //     //backgroundProcess[i]=1;

            //     printf("%s is a %d process\n", subcommands[i], backgroundProcess[i]);
            // }


            // Execute subcommands
            for (int j = 0; j < subcommand_count; j++) {
                // Tokenize subcommand arguments based on whitespace
                char *args[MAX_ARGS];
                int arg_count = 0;

                char *arg = strtok(subcommands[j], " \t");
                while (arg != NULL) {
                    args[arg_count++] = arg;
                    arg = strtok(NULL, " \t");
                }
                args[arg_count] = NULL;

                // Check for background execution with '&'
                //int background = 0;
                // if (arg_count > 0 && strcmp(args[arg_count - 1], "&") == 0) {
                //     background = 1;
                //     args[arg_count - 1] = NULL;  // Remove '&' from arguments
                // }

                // Execute the command
                if (args[0] != NULL) {

                    // if(backgroundProcess[j]){
                    //     printf("Tokenized strings for arguments are:\n");
                    //     for(int i=0; i<arg_count; i++){
                    //         printf("%s\n", args[i]);
                    //     }
                    // }

                    execute_command(args, arg_count, backgroundProcess[j], untoken);
                    
                }
            }
            
        }
        
}

