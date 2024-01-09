#include "headers.h"

extern bool pastevents_or_purge;

extern char homeDirectory[max_path];

extern char untoken[4096];

void shiftLines(char lines[MAX_LINES][MAX_LINE_LENGTH], int lineCount) {
    int i;
    for (i = 0; i < lineCount - 1; i++) {
        memset(lines[i], 0, sizeof(lines[i]));
        strcpy(lines[i], lines[i + 1]);
    }
    //memset(lines[i], 0, sizeof(lines[i]));
}

bool contains_pastevents(char *input) {    
    // it contains pastevents somewhere in the string and also does not contain execute, then do not append it
    if (strstr(input, "pastevents") != NULL && strstr(input, "execute") == NULL) {
        return true;
    }
    return false;
}

void adding_pastevents(int argCount, char *argString[]){
    // printf("wtf\n");
    // printf("%s\n", untoken);
    if(contains_pastevents(untoken)){
        return;
    }
    char lines[MAX_LINES][MAX_LINE_LENGTH];
    char newLine[MAX_LINE_LENGTH] = "";
    char pastevent_path[1040];
    snprintf(pastevent_path, sizeof(pastevent_path), "%s/%s", homeDirectory, "pastevents.txt");
    // printf("%s\n", pastevent_path);
    FILE *file = fopen(pastevent_path, "r+");       // create a path here by finding the homeDirectory of shell 
    if (file == NULL) {
        perror("Error opening file");
        return;
    }

    int lineCount = 0;
    char buffer[MAX_LINE_LENGTH];

    if(fgets(buffer, MAX_LINE_LENGTH, file) == NULL){
        // file is currently empty
        untoken[strcspn(untoken, "\n")] = '\0';
        fputs(untoken, file);
        // fputs("\n", file);
        fclose(file);
        return;
    }

    while (fgets(buffer, MAX_LINE_LENGTH, file) != NULL && lineCount < MAX_LINES) {
        size_t bufferLength = strlen(buffer);
        if (bufferLength > 0 && buffer[bufferLength - 1] == '\n') {
            buffer[bufferLength - 1] = '\0';
        }
        strcpy(lines[lineCount], buffer);
        lineCount++;
        // for(int i=0; i<lineCount; i++){
        //     printf("%s\n", lines[i]);
        // }
    }

    if(argCount == 3 && strstr(untoken, "pastevents") != NULL && strstr(untoken, "execute") != NULL){
        // only pastevents execute x has been passed, which will be dealt later
        // it will be dealt later by pastevents that is tokenized
        return;
    }
    
    // getting seg fault here
    // if(argCount >= 3 && strstr(untoken, "pastevents") != NULL && strstr(untoken, "execute") != NULL){
    //     // printf("contains pastevents execute\n");
    //     // pastevents execute is present, tokenize it first based on semi colons, replace 
    //     // pastevents execute x and then combine them back
    //     char *commands[MAX_COMMANDS];
    //     int command_count = 0;

    //     char working_copy[4096];
    //     strcpy(working_copy, untoken);

    //     char *command = strtok(working_copy, ";");
    //     while (command != NULL) {
    //         commands[command_count++] = strtrim(command);  // Trim whitespace from command
    //         //printf("%s\n", commands[command_count-1]);
    //         command = strtok(NULL, ";");
    //     }
    //     // printf("working copy: %s\n untoken: %s", working_copy, untoken);

    //     // printf("num commands separated by ; %d\n", command_count);

    //     int command_number, pastevent_loc;
    //     for (int i = 0; i < command_count; i++) {
    //         if (strstr(commands[i], "pastevents") != NULL && strstr(commands[i], "execute") != NULL) {
    //             pastevent_loc = i;
    //             char *tokens[MAX_TOKENS];
    //             int token_count = 0;
    //             // printf("pastevent location inside command:%d\n", pastevent_loc);
    //             char *token = strtok(commands[pastevent_loc], " ");
    //             while (token != NULL) {
    //                 tokens[token_count++] = token;
    //                 token = strtok(NULL, " ");
    //             }


    //             // printf("token count: %d\n", token_count);

    //             // for(int k=0; k<token_count; k++){
    //             //     printf("\n%s\n", tokens[k]);
    //             // }

    //             for (int j = 0; j < token_count; j++) {
    //                 if (strcmp(tokens[j], "execute") == 0) {
    //                     if (j + 1 < token_count) {
    //                          command_number = atoi(tokens[j + 1]);
    //                          // Convert string to integer
    //                         // printf("Extracted number: %d\n", number);
    //                         if(command_number>lineCount+1){
    //                             // printf("%d %d\n", command_number, lineCount);
    //                             printf("Incorrect usage of pastevents execute, insufficient lines in history\n");
    //                             return;
    //                         }
    //                         break;
    //                     }
    //                 }
    //             }
    //         }
    //         // printf("in file : %s\n", lines[lineCount-command_number]);
    //         strcpy(commands[pastevent_loc], lines[lineCount-command_number]);
    //         // printf("detokenized str: %s\n", commands[pastevent_loc]);
    //     }
    
    //     char updated_untokenized[MAX_COMMANDS]="";

    //     for(int i=0; i<command_count; i++){
    //         strcat(updated_untokenized, commands[i]);

    //         if(i<command_count-1){
    //             strcat(updated_untokenized, "; ");
    //         }
    //     }
    //     // printf("updated string:%s\n", updated_untokenized);

    //     if (lineCount >= MAX_LINES) {
    //         shiftLines(lines, lineCount);
    //         lineCount--; // Decrement lineCount after shifting
    //     }

    //     // probably not need it as untokenized line will be ultimately added 
    //     // rough idea but, create a list of valid commands that will be checked after the 
    //     // string is untokenized, if all the commands are valid and will give output, 
    //     // then append it to pastevents file


    //     // fix the bug here for pastevents execute 

    //     size_t newLineLength = strlen(updated_untokenized);
    //     if (newLineLength > 0 && updated_untokenized[newLineLength - 1] == '\n') {
    //         updated_untokenized[newLineLength - 1] = '\0';
    //     }
    //     fclose(file);
    //     FILE* file2 = fopen(pastevent_path, "w");
    //         if (file2 == NULL) {
    //             perror("Error opening the file");
    //             return;
    //         }
    //     fclose(file2);
    //     FILE *file1 = fopen(pastevent_path, "w");
    //     if (file1 == NULL) {
    //         perror("Error opening the file");
    //         return;
    //     }
    //     if (lineCount > 0 && strcmp(lines[lineCount - 1], updated_untokenized) != 0) {
    //         strcpy(lines[lineCount], updated_untokenized);

    //         fseek(file, 0, SEEK_SET);
    //         for (int i = 0; i < lineCount + 1; i++) {
    //             fputs(lines[i], file1);
    //             if (i != lineCount) {
    //                 fputs("\n", file1);
    //             }
    //         }
    //     }

    //    fclose(file1);
    //    return;
    // }


//***********************************



    fclose(file);
    // printf("reached here 140\n");
    file = fopen(pastevent_path, "r+");       // create a path here by finding the homeDirectory of shell 
    if (file == NULL) {
        perror("Error opening file");
        return;
    }

    lineCount = 0;
    char buffer2[MAX_LINE_LENGTH];

    while (fgets(buffer2, MAX_LINE_LENGTH, file) != NULL && lineCount < MAX_LINES) {
        size_t bufferLength = strlen(buffer2);
        if (bufferLength > 0 && buffer2[bufferLength - 1] == '\n') {
            buffer2[bufferLength - 1] = '\0';
        }
        // printf("%s\n",buffer);
        strcpy(lines[lineCount], buffer2);
        lineCount++;
        
    }
    // for(int i=0;i<lineCount;i++){
    //         printf("%s\n",lines[i]);
    //     }
    fclose(file);

    if (lineCount >= MAX_LINES) {
        //printf("hi\n");
        shiftLines(lines, lineCount);

        for(int i=0; i<strlen(lines[lineCount-1]); i++){
            lines[lineCount-1][i]='\0';
        }
        lineCount--; // Decrement lineCount after shifting
        // for(int i=0;i<lineCount;i++){
        //     printf("%s\n",lines[i]);
        // }
    }

    // probably not need it as untokenized line will be ultimately added 
    // rough idea but, create a list of valid commands that will be checked after the 
    // string is untokenized, if all the commands are valid and will give output, 
    // then append it to pastevents file


    // for (int i = 1; i < argCount; i++) {
    //     strcat(newLine, argString[i]);
    //     if (i != argCount - 1) {
    //         strcat(newLine, " ");
    //     }
    // }
    // for(int i=0;i<lineCount;i++){
    //         printf("%s\n",lines[i]);
    //     }
    strcpy(newLine, untoken);

    size_t newLineLength = strlen(newLine);
    newLine[strcspn(newLine, "\n")]='\0';
    // if (newLineLength > 0 && newLine[newLineLength - 1] == '\n') {
    //     newLine[newLineLength] = '\0';
    // }

    

    file = fopen(pastevent_path, "w");
        if (file == NULL) {
            perror("Error opening the file");
            return;
        }
    fclose(file);

    //printf("\n\n %s\n\n", );


    file = fopen(pastevent_path, "w");

    if (lineCount > 0 && strcmp(lines[lineCount - 1], newLine) != 0) {
        memset(lines[lineCount], 0, MAX_LINE_LENGTH);

        if (strlen(lines[lineCount]) > strlen(newLine)) {
            memset(lines[lineCount], 0, MAX_LINE_LENGTH);
        }
        // printf("%s\n", newLine);
        strcpy(lines[lineCount], newLine);

        fseek(file, 0, SEEK_SET);

        for (int i = 0; i < lineCount + 1; i++) {
            fputs(lines[i], file);
            if (i != lineCount) {
                fputs("\n", file);
            }
        }
    }

    fclose(file);
}

int pastevents(int argCount, char *argString[]) {
    char pastevent_path[1040];
    snprintf(pastevent_path, sizeof(pastevent_path), "%s/%s", homeDirectory, "pastevents.txt");
    
    if (argCount == 1) {
        // only pastevents has been passed, so print the txt file
        // printf("Usage: %s <new_line_word1> <new_line_word2> ...\n", argString[0]);
        // return 1;

            FILE *file = fopen(pastevent_path, "r");
            if (file == NULL) {
                perror("Error opening the file");
                return 1;
            }

            // Read and print the file contents
            char buffer[1024];
            size_t bytes_read;

            while ((bytes_read = fread(buffer, 1, sizeof(buffer), file)) > 0) {
                buffer[bytes_read++]='\n';
                if (fwrite(buffer, 1, bytes_read, stdout) != bytes_read) {
                    perror("Error writing to stdout");
                    fclose(file);
                    return 1;
                }
            }
            fclose(file);
    }

    else if (argCount == 2) {

        if(strcmp(argString[1], "purge")==0){
            // pastevents purge has been passed
            FILE *file = fopen(pastevent_path, "w");
                if (file == NULL) {
                    perror("Error opening the file");
                    return 1;
                }
            fclose(file);
        }
        else{
            printf("Incorrect format for pastevents\n");
            return 1;
        }

        
    }

    else if (argCount == 3) {
        // pastevents execute was passed
        char lines[MAX_LINES][MAX_LINE_LENGTH];
        char newLine[MAX_LINE_LENGTH] = "";

        FILE *file = fopen(pastevent_path, "r+");       // create a path here by finding the homeDirectory of shell 
        if (file == NULL) {
            perror("Error opening file");
            return 1;
        }

        int lineCount = 0;
        char buffer[MAX_LINE_LENGTH];

        while (fgets(buffer, MAX_LINE_LENGTH, file) != NULL && lineCount < MAX_LINES) {
            size_t bufferLength = strlen(buffer);
            if (bufferLength > 0 && buffer[bufferLength - 1] == '\n') {
                buffer[bufferLength - 1] = '\0';
            }
            strcpy(lines[lineCount], buffer);
            lineCount++;
        }


        //for now putting fclose here so that you can modify pastevents execute later
        fclose(file);

        // printf("line count: %d\n", lineCount);

        if (atoi(argString[2]) <= lineCount + 1) {
            // valid pastevent execute request
            // tokenize(untokenized_str);
            // execute_command(lines[lineCount - argString[2]], 0);       // under the assumption that background processes will not be stored in pastevents
            // currently incorrect arg passed since this string is not tokenized, so probably create a tokenize function which will first tokenize 
            // the whole string and then add the untokenized string which does not contain pastevents or pastevents purge... 
            tokenize(lines[lineCount-atoi(argString[2])]);

            // make a flag for append if all the commands fit the criteria (check if we can get any signal after the process has terminated to check whether the command was correct or not)
        }
        else {
            printf("Incorrect usage of pastevents execute, insufficient lines in history\n");
        }
    }
    return 0;

}
