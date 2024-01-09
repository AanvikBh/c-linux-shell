#include "headers.h"

int shellInvokes=0;
int break_signal;

int standard_stdin;
int standard_stdout;

List backgroundlist;
List activitiesList;
List foregroundList;

char homeDirectory[max_path];
char previousDirectory[max_path];
char currentDirectory[max_path];
bool pastevents_or_purge = 0;
char executing_command[32];
char temp_home[max_path];
char untoken[4096];

pid_t child_parent_pid;
pid_t terminal_pid;
pid_t currentForeground=0;

char input_file[max_name_len];
char output_file[max_name_len];
char output_append_file[max_name_len];


int main()
{   terminal_pid=getpid();
    // Keep accepting commands
    backgroundlist=CreateEmptyList();
    foregroundList=CreateEmptyList();
    //activitiesList=CreateEmptyList();
    
    signal(SIGINT, ctrlC_handler);
    signal(SIGTSTP, ctrlZ_handler);
    signal(SIGQUIT, ctrlD_handler);

    signal(SIGUSR1, switch_bg_to_fg);
    // signal(SIGQUIT, SIG_IGN);
    



    while (1){
        
        shellInvokes++;
        // Print appropriate prompt with username, systemname and directory before accepting input
        prompt();
        char input[4096];
        fgets(input, 4096, stdin);
        // if(strlen(input)==1){
        //     if(input[0]=='4'){
        //         //ctrl D
        //     }
        //     if(input[0]=='3'){
        //         //ctrl C
        //     }
        //     if(strcmp(input, "26"))
        // }
        // strcpy(input,"echo Lorem Ipsum");
        strcpy(untoken, input);


        hasProcessTerminated(backgroundlist);
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
            //printf("background count %d\n", background_count);
            // Tokenize command based on '&'
            char *subcommand = strtok(commands[i], "&");
            while (subcommand != NULL) {
                subcommands[subcommand_count++] = strtrim(subcommand);  // Trim whitespace from subcommand
                subcommand = strtok(NULL, "&");
            }
            
            for (int k = 0; k < subcommand_count; k++){
                if(k<background_count){
                    backgroundProcess[k] = 1;
                }
                else{
                    backgroundProcess[k] = 0;
                }

            }
            // printf("subcommand count%d\n", subcommand_count);
            // for(int i=0; i<subcommand_count; i++){
            //     // all processes tokenized by & should be in background except the very last one
                
            //     //backgroundProcess[i]=1;
            //     printf("identifier: %d\n", backgroundProcess[i]);
            //     //printf("%s is a %d process\n", subcommands[i], backgroundProcess[i]);
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

                    execute_command(args, arg_count, backgroundProcess[j], input);
                    if(break_signal==1){
                        break;
                    }
                }
            }
            if(break_signal==1){
                break;
            }
        }
        if(break_signal==1){
            break;
        }
    }
}

    
