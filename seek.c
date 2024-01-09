#include "headers.h"

extern char homeDirectory[max_path];
extern char previousDirectory[max_path];
extern char currentDirectory[max_path];
extern char temp_home[max_path];

void get_relative_path2(const char *initial, const char *current, char *relative_path) {
    if (strncmp(initial, current, strlen(initial)) == 0) {
        strcpy(relative_path, ".");
        strcat(relative_path, current + strlen(initial));
    } else {
        strcpy(relative_path, current);
    }
}

void print_colored(char *path, int is_directory, char* home_path) {
    char* relative=(char*)malloc(sizeof(char)*1024);
    get_relative_path2(home_path, path, relative);
    if (is_directory) {
        printf("\x1b[34m%s\x1b[0m\n", relative); // Blue color
    } else {
        printf("\x1b[32m%s\x1b[0m\n", relative); // Green color
    }
    free(relative);
}

void seek_file(char *current_dir, char *target_file, int *numFiles, int *numDirectories, char* uniquePathFile) {
    DIR *dir = opendir(current_dir);
    if (dir == NULL) {
        perror("Error opening directory");
        return;
    }

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_type == DT_REG) {  // Check if the entry is a regular file
            if (strcmp(entry->d_name, target_file) == 0 || (strstr(entry->d_name, target_file) != NULL && strstr(entry->d_name, ".txt") != NULL)) {
                char file_path[MAX_PATH_LEN];
                snprintf(file_path, sizeof(file_path), "%s/%s", current_dir, entry->d_name);
                (*numFiles)++;
                strcpy(uniquePathFile, file_path);
                

                print_colored(file_path, 0, temp_home); // Non-target directory paths in green
            }
        } else if (entry->d_type == DT_DIR) {
            if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
                continue;
            }
            char next_dir[MAX_PATH_LEN];
            snprintf(next_dir, sizeof(next_dir), "%s/%s", current_dir, entry->d_name);
            if (strcmp(entry->d_name, target_file) == 0) {
                (*numDirectories)++;
                // print_colored(next_dir, 1); // Target directory paths in blue
            }
            seek_file(next_dir, target_file, numFiles, numDirectories, uniquePathFile);
        }
    }
    
    closedir(dir);
}

void seek_dir(char *current_dir, char *target_dir, int *numDirectories, char* uniquePathDirectory) {
    DIR *dir = opendir(current_dir);
    if (dir == NULL) {
        perror("Error opening directory");
        return;
    }

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_type == DT_DIR) {
            if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
                continue;
            }
            char next_dir[MAX_PATH_LEN];
            snprintf(next_dir, sizeof(next_dir), "%s/%s", current_dir, entry->d_name);
            if (strcmp(entry->d_name, target_dir) == 0) {
                strcpy(uniquePathDirectory, next_dir);
                (*numDirectories)++;
                print_colored(next_dir, 1, temp_home); // Target directory paths in blue
            }
            seek_dir(next_dir, target_dir, numDirectories, uniquePathDirectory);
        }
    }
    closedir(dir);
}

int seek(int argCount, char* argString[]){

    strcpy(temp_home, currentDirectory);
    if(argCount<2){
        printf("Incorrect usage! seek <flags> <search> <target_directory>\n");
        return 1;
    }
    char starting_dir[MAX_PATH_LEN];
    // printf("%d\n", argCount);
    if(argCount==2){
        //search for both directories and files from home directory
        int nDir = 0;
        int nFile = 0;
        char uniquePathDirectory[MAX_PATH_LEN];
        char uniquePathFile[MAX_PATH_LEN];
        seek_dir(".", argString[1], &nDir, uniquePathDirectory);
        // if(nDir==0){
        //     printf("No matches found!\n");
        // }

        nDir=0;
        nFile=0;
        seek_file(".", argString[1], &nFile, &nDir, uniquePathFile);
        if(nFile==0 && nDir==0){
            printf("No matches found!\n");
        }
    }

    int directory_only = 0;
    int file_only = 0;
    int e_only = 0;

    int nDir = 0;
    int nFile = 0;
    char uniquePathDirectory[MAX_PATH_LEN];
    char uniquePathFile[MAX_PATH_LEN];


    if(argCount==3){

        for (int i = 1; i < argCount; i++) {
            if (strcmp(argString[i], "-d") == 0) {
                directory_only = 1;
            } else if (strcmp(argString[i], "-f") == 0) {
                file_only = 1;
            } else if (strcmp(argString[i], "-e") == 0 ) {
                e_only = 1;
            } 
            else if (strcmp(argString[i], "-ef") == 0 || strcmp(argString[i], "-fe") == 0){
                e_only = 1;
                file_only = 1;
            }
            else if (strcmp(argString[i], "-ed") == 0 || strcmp(argString[i], "-de") == 0){
                e_only = 1;
                directory_only = 1;
            }
            else if (argString[1][0]!='-'){
                // a flag has not been passed, so perform search on all with given directory to start
                seek_dir(argString[2], argString[1], &nDir, uniquePathDirectory);

                nDir=0;
                seek_file(argString[2], argString[1], &nFile, &nDir, uniquePathFile);

                if(nDir==0 && nFile==0){
                    printf("No matches found!\n");
                }

                nDir=0;
                nFile=0;
            }
            // else{
            //     printf("Invalid Flags!\n");
            //     return 1;
            // }
        }
        // all possible flags: -d, -f, -e, -de/ed, -ef/fe
        // all possible double flags: -d -e, -e -d, -e -f, -f -e
        // throw error at: -d -f, any other single flag, -ee

        // FIGURE OUT HOW TO DEAL WITH FLAGS ONCE


        // either target directory with no flags is given or a flag with home directory is given
        

        if(directory_only && !file_only && !e_only){
            //search for a directory with a given name from home
            seek_dir(".", argString[2], &nDir, uniquePathDirectory);
            if(nDir==0){
                printf("No matches found!\n");
            }
            nDir=0;
        }

        if(!directory_only && file_only && !e_only){
            //search for a file with a given name from home
            seek_file(".", argString[2], &nFile, &nDir, uniquePathFile);
            if(nFile==0){
                printf("No matches found!\n");
            }
            nDir=0;
            nFile=0;
        }

            
        if(strcmp(argString[1], "-ef")==0 || strcmp(argString[1], "-fe")==0){
            seek_file(".", argString[2], &nFile, &nDir, uniquePathFile);
            if(nFile==0){
                printf("No matches found!\n");
            }
            nFile=0;
            nDir=0;
            if(access(uniquePathFile, R_OK) != 0){
                printf("Missing permissions for task!\n");
                return 1;
            }
            FILE *file = fopen(uniquePathFile, "r");
            if (file == NULL) {
                perror("Error opening file");
                return 1;
            }
            
            // Read and print the contents of the file
            // printf("Contents of the file:\n");
            char buffer[1024]; // Buffer to hold the read content
            
            while (fgets(buffer, sizeof(buffer), file) != NULL) {
                puts(buffer); // Print the line to the screen
            }
            
            // Close the file
            fclose(file);
        }
        if(strcmp(argString[1], "-ed")==0 || strcmp(argString[1], "-de")==0){
            seek_dir(".", argString[2], &nDir, uniquePathDirectory);
            if(nDir==0){
                printf("No matches found!\n");
            }
            nDir=0;
            if(access(uniquePathDirectory, X_OK) != 0){
                printf("Missing permissions for task!\n");
                return 1;
            }
            if (chdir(uniquePathDirectory) == 0) {
                strcpy(previousDirectory, currentDirectory);
                //printf("Current directory: %s\n", );
                getcwd(currentDirectory, max_path);
                strcpy(temp_home, currentDirectory);    
                return 0;
            }
        }
    }

        if(argCount == 4){
            // either 2 flags and home directory have been passed 
            // or one flag and start directory have been passed 

            if(argString[1][0]=='-' && argString[2][0]=='-'){
                // both flags
                for (int i = 1; i < 3; i++) {
                    if (strcmp(argString[i], "-d") == 0) {
                        directory_only = 1;
                    } else if (strcmp(argString[i], "-f") == 0) {
                        file_only = 1;
                    } else if (strcmp(argString[i], "-e") == 0 ) {
                        e_only = 1;
                    } 
                    else if (strcmp(argString[i], "-ef") == 0 || strcmp(argString[i], "-fe") == 0){
                        e_only = 1;
                        file_only = 1;
                    }
                    else if (strcmp(argString[i], "-ed") == 0 || strcmp(argString[i], "-de") == 0){
                        e_only = 1;
                        directory_only = 1;
                    }
                    // else if (argString[1][0]!='-'){
                    //     // a flag has not been passed, so perform search on all with given directory to start
                    //     seek_dir(argString[3], argString[1], &nDir, uniquePathDirectory);
                    //     nDir=0;
                    //     seek_file(argString[2], argString[1], &nFile, &nDir, uniquePathFile);
                    //     nDir=0;
                    //     nFile=0;
                    // }
                    else{
                        printf("Invalid Flags!\n");
                        return 1;
                    }
                }
                if(directory_only && e_only){
                    seek_dir(".", argString[3], &nDir, uniquePathDirectory);
                    if(nDir==0){
                        printf("No matches found!\n");
                    }
                    nFile=0;
                    nDir=0;
                    if(access(uniquePathDirectory, X_OK) != 0){
                        printf("Missing permissions for task!\n");
                        return 1;
                    }
                    if (chdir(uniquePathDirectory) == 0) {
                        strcpy(previousDirectory, currentDirectory);
                        getcwd(currentDirectory, max_path);
                        strcpy(temp_home, currentDirectory);
                        return 0;
                    }
                }
                else if(file_only && e_only){
                    seek_file(".", argString[3], &nFile, &nDir, uniquePathFile);
                    if(nFile==0){
                        printf("No matches found!\n");
                    }
                    nFile=0;
                    nDir=0;
                    if(access(uniquePathFile, R_OK) != 0){
                        printf("Missing permissions for task!\n");
                        return 1;
                    }
                    FILE *file = fopen(uniquePathFile, "r");
                    if (file == NULL) {
                        perror("Error opening file");
                        return 1;
                    }
                    // Read and print the contents of the file
                    // printf("Contents of the file:\n");
                    char buffer[1024]; // Buffer to hold the read content
                    while (fgets(buffer, sizeof(buffer), file) != NULL) {
                        puts(buffer); // Print the line to the screen
                    }
                    
                    // Close the file
                    fclose(file);
                }

            }
            else if(argString[1][0] == '-' && argString[2][0]!='-') {
                // only one flag and start directory given
                for (int i = 1; i < 2; i++) {
                    if (strcmp(argString[i], "-d") == 0) {
                        directory_only = 1;
                    } else if (strcmp(argString[i], "-f") == 0) {
                        file_only = 1;
                    } else if (strcmp(argString[i], "-e") == 0 ) {
                        e_only = 1;
                    } 
                    else if (strcmp(argString[i], "-ef") == 0 || strcmp(argString[i], "-fe") == 0){
                        e_only = 1;
                        file_only = 1;
                    }
                    else if (strcmp(argString[i], "-ed") == 0 || strcmp(argString[i], "-de") == 0){
                        e_only = 1;
                        directory_only = 1;
                    }
                    // else if (argString[1][0]!='-'){
                    //     // a flag has not been passed, so perform search on all with given directory to start
                    //     seek_dir(argString[3], argString[1], &nDir, uniquePathDirectory);
                    //     nDir=0;
                    //     seek_file(argString[2], argString[1], &nFile, &nDir, uniquePathFile);
                    //     nDir=0;
                    //     nFile=0;
                    // }
                    else{
                        printf("Invalid Flags!\n");
                        return 1;
                    }
                }
                if(directory_only && !file_only && !e_only){
                    //search for a directory with a given name from home
                    seek_dir(argString[3], argString[2], &nDir, uniquePathDirectory);
                    if(nDir==0){
                        printf("No matches found!\n");
                    }
                    nFile=0;
                    nDir=0;
                }

                if(!directory_only && file_only && !e_only){
                    //search for a file with a given name from home
                    seek_file(argString[3], argString[2], &nFile, &nDir, uniquePathFile);
                    if(nFile==0){
                        printf("No matches found!\n");
                    }
                    nFile=0;
                    nDir=0;
                }
                    
                if(strcmp(argString[1], "-ef")==0 || strcmp(argString[1], "-fe")==0){
                    seek_file(argString[3], argString[2], &nFile, &nDir, uniquePathFile);
                    if(nFile==0){
                        printf("No matches found!\n");
                    }
                    nFile=0;
                    nDir=0;
                    if(access(uniquePathFile, R_OK) != 0){
                        printf("Missing permissions for task!\n");
                        return 1;
                    }
                    FILE *file = fopen(uniquePathFile, "r");
                    if (file == NULL) {
                        perror("Error opening file");
                        return 1;
                    }
                    
                    // Read and print the contents of the file
                    // printf("Contents of the file:\n");
                    char buffer[1024]; // Buffer to hold the read content
                    while (fgets(buffer, sizeof(buffer), file) != NULL) {
                        puts(buffer); // Print the line to the screen
                    }
                    
                    // Close the file
                    fclose(file);
                }
                if(strcmp(argString[1], "-ed")==0 || strcmp(argString[1], "-de")==0){
                    seek_dir(argString[3], argString[2], &nDir, uniquePathDirectory);
                    if(nDir==0){
                        printf("No matches found!\n");
                    }
                    nFile=0;
                    nDir=0;
                    if(access(uniquePathDirectory, X_OK) != 0){
                        printf("Missing permissions for task!\n");
                        return 1;
                    }
                    if (chdir(uniquePathDirectory) == 0) {
                        strcpy(previousDirectory, currentDirectory);
                        getcwd(currentDirectory, max_path);
                        strcpy(temp_home, currentDirectory);
                        return 0;
                    }
                }
            }

        }
    //printf("%d\n", argCount);
    if(argCount == 5){
        // 2 flags and target directory given
        if(strcmp(argString[1], "-d")==0 && strcmp(argString[2], "-f")==0 || strcmp(argString[1], "-f")==0 && strcmp(argString[2], "-d")==0){
            printf("Invalid flags!\n");
            return 1;
        }
        for (int i = 1; i < argCount; i++) {
            if (strcmp(argString[i], "-d") == 0) {
                directory_only = 1;
            } else if (strcmp(argString[i], "-f") == 0) {
                file_only = 1;
            } else if (strcmp(argString[i], "-e") == 0 ) {
                e_only = 1;
            } 
            else if (strcmp(argString[i], "-ef") == 0 || strcmp(argString[i], "-fe") == 0){
                e_only = 1;
                file_only = 1;
            }
            else if (strcmp(argString[i], "-ed") == 0 || strcmp(argString[i], "-de") == 0){
                e_only = 1;
                directory_only = 1;
            }
        }
        //printf("%d %d %d\n", directory_only, file_only, e_only);
        if(directory_only && e_only){
            seek_dir(argString[4], argString[3], &nDir, uniquePathDirectory);
                if(nDir==0){
                    printf("No matches found!\n");
                }
                nFile=0;
                nDir=0;
            if(access(uniquePathDirectory, X_OK) != 0){
                printf("Missing permissions for task!\n");
                return 1;
            }
            if (chdir(uniquePathDirectory) == 0) {
                    strcpy(previousDirectory, currentDirectory);
                    getcwd(currentDirectory, 1024);
                    strcpy(temp_home, currentDirectory);
                    return 0;
                }
        }
        else if(file_only && e_only){
            seek_file(argString[4], argString[3], &nFile, &nDir, uniquePathFile);
                if(nFile==0){
                    printf("No matches found!\n");
                }
                nFile=0;
                nDir=0;
            if(access(uniquePathFile, R_OK) != 0){
                printf("Missing permissions for task!\n");
                return 1;
            }
            FILE *file = fopen(uniquePathFile, "r");
            if (file == NULL) {
                perror("Error opening file");
                return 1;
            }
            // Read and print the contents of the file
            // printf("Contents of the file:\n");
            char buffer[1024]; // Buffer to hold the read content
            while (fgets(buffer, sizeof(buffer), file) != NULL) {
                puts(buffer); // Print the line to the screen
            }
            
            // Close the file
            fclose(file);
        }
        
    }
    else if(argCount>5){
        printf("Passed too many arguments in seek, follow this format: seek <flags> <search> <target_directory>\n");
        return 1;
    }
    return 0;
}
    
