#include "headers.h"

extern char homeDirectory[max_path];
extern char previousDirectory[max_path];
extern char currentDirectory[max_path];

int peek(int argCount, char *argString[]) {
    

    int show_hidden = 0;
    int show_details = 0;
    char path[PATH_MAX] = ".";

    for (int i = 1; i < argCount; i++) {
        if (strcmp(argString[i], "-a") == 0) {
            show_hidden = 1;
        } else if (strcmp(argString[i], "-l") == 0) {
            show_details = 1;
        } else if (strcmp(argString[i], "-la") == 0 || strcmp(argString[i], "-al") == 0) {
            show_hidden = 1;
            show_details = 1;
        } else {
            if (argString[i][0] == '-' && i <= 3) {
                for (int j = 1; j < strlen(argString[i]); j++) {
                    if (argString[i][j] != 'l' && argString[i][j] != 'a') {
                        printf("Invalid Flags entered\n");
                        return 1;
                    }
                }
            } else if (i==argCount && strcmp(argString[i], "-") == 0) {
                printf("- is not a valid path for peek\n");
                return 1;
            } else {
                if (argString[i][0] == '~') {
                    strcpy(path, homeDirectory);
                    strcat(path, argString[i] + 1);
                } else {
                    strcpy(path, argString[i]);
                }
            }
        }
    }
    char new_path[MAX_INPUT_LENGTH];
    memset(new_path, 0, sizeof(new_path));
    //printf("%s\n", path);
    // if (strcmp(path, "-")==0){
    //     printf("- is not a valid path for peek\n");
    //     return 1;
    // }
    if (path[0] == '~') {
        if (strlen(path) == 1 || path[1] == '/') {
            // printf("%s\n", homeDirectory);
            strcpy(path, homeDirectory);
            strcat(new_path, path + 1);
            // printf("1    %s\n", new_path);
        } else if (strstr(currentDirectory, homeDirectory) == currentDirectory) {
            strcpy(new_path, homeDirectory);
            strcat(new_path, "/");
            strcat(new_path, path + 2);
            // printf("2    %s\n", new_path);
        } else {
            printf("Invalid path: %s\n", path);
            return 1;
        }
    } else if (strcmp(path, ".") == 0) {
        strcpy(new_path, currentDirectory);
        // printf("3    %s\n", new_path);
    } else if (strcmp(path, "..") == 0) {
        if (strcmp(currentDirectory, homeDirectory) == 0) {
            strcpy(new_path, homeDirectory);
            // printf("4    %s\n", new_path);
        }
        else {
            strcpy(new_path, currentDirectory);
            get_parent_directory(new_path);
            // printf("5    %s\n", new_path);
            // add the directory in which it was before
        }
     } // else if (path[0] == '-') {
    //     // not working for this
    //     char tempPreviousDirectory[PATH_MAX];
    //     strcpy(tempPreviousDirectory, previousDirectory);
    //     strcpy(new_path, tempPreviousDirectory);
    //     strcpy(tempPreviousDirectory, currentDirectory);

    //     // printf("6    %s\n", new_path);
    // } 
    else {
        strcpy(new_path, path);
        //printf("7    %s\n", new_path);
    }
    list_files_and_directories(new_path, show_hidden, show_details);

    return 0;
}

int compareFileInfo(const void *a, const void *b) {
    const char *fileInfoA = *(const char **)a;
    const char *fileInfoB = *(const char **)b;

    // Compare strings lexicographically
    return strcmp(fileInfoA, fileInfoB);
}


char* getMonth(int month){
    switch (month)
    {
    case 1:
        return "Jan";
        /* code */
        break;

    case 2:
        return "Feb";
        /* code */
        break;

    case 3:
        return "Mar"; 
        /* code */
        break;

    case 4:
        return "Apr";
        break;

    case 5:
        return "May";
        /* code */
        break;

    case 6:
        return "Jun";
        /* code */
        break;

    case 7:
        return "Jul";
        /* code */
        break;
    
    case 8:
        return "Aug";
        /* code */
        break;

    case 9:
        return "Sep";
        break;

    case 10:
        return "Oct";
        break;

    case 11:
        return "Nov";
        break;

    case 12:
        return "Dec";
        break;

    default:
        return NULL;
        break;
    }
}

char* permissions(mode_t mode) {
    char *modeval = malloc(sizeof(char) * 12);
    if (modeval != NULL) {
        modeval[0] = S_ISDIR(mode) ? 'd' :
                     S_ISLNK(mode) ? 'l' :
                     '-';
        modeval[1] = (mode & S_IRUSR) ? 'r' : '-';
        modeval[2] = (mode & S_IWUSR) ? 'w' : '-';
        modeval[3] = (mode & S_IXUSR) ? 'x' : '-';
        modeval[4] = (mode & S_IRGRP) ? 'r' : '-';
        modeval[5] = (mode & S_IWGRP) ? 'w' : '-';
        modeval[6] = (mode & S_IXGRP) ? 'x' : '-';
        modeval[7] = (mode & S_IROTH) ? 'r' : '-';
        modeval[8] = (mode & S_IWOTH) ? 'w' : '-';
        modeval[9] = (mode & S_IXOTH) ? 'x' : '-';
        modeval[10] = '@';
        modeval[11] = '\0';
    }
    return modeval;
}

char* getOwner(struct stat entry_stat) {
    struct passwd *owner_info = getpwuid(entry_stat.st_uid);
    return owner_info->pw_name;
}

char* getGroup(struct stat entry_stat) {
    struct group *group_info = getgrgid(entry_stat.st_gid);
    return group_info->gr_name;
}

void list_files_and_directories(const char *path, int show_hidden, int show_details) {
    struct dirent *entry;
    DIR *dir = opendir(path);

    if (!dir) {
        perror("opendir");
        return;
    }

    int total_blocks = 0;
    int numFiles = 0;   //hidden files are also included 

    while ((entry = readdir(dir)) != NULL) {
        
        if (!show_hidden && entry->d_name[0] == '.') {
            //numFiles++;
            continue;
        }
        numFiles++;
        char entry_path[PATH_MAX];
        snprintf(entry_path, sizeof(entry_path), "%s/%s", path, entry->d_name);

        struct stat entry_stat;
        if (lstat(entry_path, &entry_stat) == -1) {
            perror("lstat");
            continue;
        }
        total_blocks += entry_stat.st_blocks;
    }
    
    if(show_details){
        printf("total: %d\n", total_blocks);
    }
    closedir(dir);

    char** nameStringArr = (char**) malloc(sizeof(char *) * numFiles);

    int i = 0;

    dir = opendir(path);
    if (!dir) {
        perror("opendir");
        return;
    }
    while ((entry = readdir(dir)) != NULL) {
        if (!show_hidden && entry->d_name[0] == '.') {
            continue;
        }

        char entry_path[PATH_MAX];
        snprintf(entry_path, sizeof(entry_path), "%s/%s", path, entry->d_name);

        struct stat entry_stat;
        if (lstat(entry_path, &entry_stat) == -1) {
            perror("lstat");
            continue;
        }
        
        // Allocate memory for the current entry's name
        nameStringArr[i] = malloc(strlen(entry->d_name) + 1);
        if (nameStringArr[i] == NULL) {
            perror("malloc");
            // Handle memory allocation error
            continue;
        }
        
        strcpy(nameStringArr[i], entry->d_name);
        i++;
    }

    closedir(dir);

    qsort(nameStringArr, numFiles, sizeof(char*), compareFileInfo);

    // Print sorted fileArray here...
    // struct dirent *entry2;
    dir = opendir(path);
    if (!dir) {
        perror("opendir");
        return;
    }

        for (int i = 0; i < numFiles; i++) {
        char entry_path[PATH_MAX];
        snprintf(entry_path, sizeof(entry_path), "%s/%s", path, nameStringArr[i]);

        struct stat entry_stat;
        if (lstat(entry_path, &entry_stat) == -1) {
            perror("lstat");
            continue;
        }

        if (S_ISDIR(entry_stat.st_mode) && !show_details) {
            printf("\033[34m%s\033[0m  ", nameStringArr[i]); // Blue for directories
        } else if (S_ISDIR(entry_stat.st_mode) && show_details) {
            struct tm *creation_time = localtime(&entry_stat.st_ctime);
            printf("\033[34m%s %lu %s %s %ld %s %2d %02d:%02d %s\033[0m  ", permissions(entry_stat.st_mode), entry_stat.st_nlink, getOwner(entry_stat), getGroup(entry_stat), entry_stat.st_size, getMonth(creation_time->tm_mon + 1), creation_time->tm_mday, creation_time->tm_hour, creation_time->tm_min, nameStringArr[i]); // Blue for directories
        } else if (entry_stat.st_mode & S_IXUSR && !show_details) {
            printf("\033[32m%s\033[0m  ", nameStringArr[i]); // Green for executables
        } else if (entry_stat.st_mode & S_IXUSR && show_details) {
            struct tm *creation_time = localtime(&entry_stat.st_ctime);
            printf("\033[32m%s %lu %s %s %ld %s %2d %02d:%02d %s\033[0m  ", permissions(entry_stat.st_mode), entry_stat.st_nlink, getOwner(entry_stat), getGroup(entry_stat), entry_stat.st_size, getMonth(creation_time->tm_mon + 1), creation_time->tm_mday, creation_time->tm_hour, creation_time->tm_min, nameStringArr[i]); // Green for executables
        } else {
            if (!show_details) {
                printf("%s  ", nameStringArr[i]); // Regular files
            }
            if (show_details) {
                struct tm *creation_time = localtime(&entry_stat.st_ctime);
                printf("%s %lu %s %s %ld %s %2d %02d:%02d %s  ", permissions(entry_stat.st_mode), entry_stat.st_nlink, getOwner(entry_stat), getGroup(entry_stat), entry_stat.st_size, getMonth(creation_time->tm_mon + 1), creation_time->tm_mday, creation_time->tm_hour, creation_time->tm_min, nameStringArr[i]);
            }
        }

        printf("\n");
    }
     closedir(dir);
     for (int i = 0; i < numFiles; i++) {
        free(nameStringArr[i]); // Free memory for each string
    }
    free(nameStringArr);
}
