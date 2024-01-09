#include "headers.h"

extern char homeDirectory[max_path];
extern char previousDirectory[max_path];
extern char currentDirectory[max_path];

void get_home_directory(char *home_path) {
    struct passwd *pw;
    uid_t uid;
    uid = geteuid();
    pw = getpwuid(uid);
    strcpy(home_path, pw->pw_dir);
}

void get_parent_directory(char *path) {
    char *last_slash = strrchr(path, '/');
    if (last_slash != NULL) {
        *last_slash = '\0';  // Remove the last directory and slash
    }
}


int warp_driver(const char *path, char *home_path) {
    char new_path[max_path];
    if (path[0] ==  '/'){
        //absolute path, so copy it as it is
        
        strcpy(new_path, path);
        //printf("%s\n", new_path);
    }
    else if (path[0] == '~') {
        if (strlen(path) == 1 || path[1] == '/') {
            strcpy(new_path, home_path);
            strcat(new_path, path + 1);
        } else if (strstr(currentDirectory, home_path) == currentDirectory) {
            strcpy(new_path, home_path);
            strcat(new_path, "/");
            strcat(new_path, path + 2);
        } else {
            printf("Invalid path: %s\n", path);
            return -1;
        }
    } else if (strcmp(path, ".") == 0) {
        strcpy(new_path, currentDirectory);
    } else if (strcmp(path, "..") == 0) {
        strcpy(new_path, currentDirectory);
        get_parent_directory(new_path);
    } else if (path[0] == '-') {
        strcpy(new_path, previousDirectory);
    } else {
        
        if (strstr(home_path, path) == home_path) {
            strcpy(new_path, home_path);
            strcat(new_path, "/");
            strcat(new_path, path);
        } else {
            strcpy(new_path, path);
        }
    }

    if (chdir(new_path) == 0) {
        
        strcpy(previousDirectory, currentDirectory);
        strcpy(currentDirectory, new_path);
        printf("Current directory: %s\n", getcwd(currentDirectory, max_path));
        return 0;
    } else {
        perror("chdir");
        return -1;
    }
}


int warp(char *args[], int countArgs, char *home_path) {
    char prevPath[max_path];
    getcwd(prevPath, sizeof(prevPath));
    int driver_return = 0;  // Initialize to success

    if(countArgs==1){
        //warp to home directory 
        driver_return=warp_driver("~", home_path);
        //currently it is not printing the path to home    
    }
    else{
        for (int i = 1; i < countArgs; i++) {
        driver_return = warp_driver(args[i], home_path);
            if (driver_return != 0) {
                break;  // Exit loop on error
            }
        }
    }
    
    return driver_return;
}


