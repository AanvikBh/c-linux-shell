#include "headers.h"

extern int shellInvokes;
extern char homeDirectory[max_path];
extern char previousDirectory[max_path];
extern char currentDirectory[max_path];

void get_relative_path(const char *initial, const char *current, char *relative_path) {
    if (strncmp(initial, current, strlen(initial)) == 0) {
        strcpy(relative_path, "~");
        strcat(relative_path, current + strlen(initial));
    } else {
        strcpy(relative_path, current);
    }
}

void prompt() {
    struct utsname system_info;

    if (uname(&system_info) == -1) {
        perror("uname");
        // return 1;
    }
    struct passwd *pw;
    uid_t uid;
    int c;

    uid = geteuid();
    pw = getpwuid(uid);
    if (!pw) {
        exit(EXIT_FAILURE);
    }
    char cwd[max_path]; // Allocate a buffer for the current directory path

    if (shellInvokes == 1) {
        // this is the first time that shell has been invoked, so set the home directory
        if (!getcwd(cwd, sizeof(cwd))) {
            perror("getcwd() error");
        }
        strcpy(homeDirectory, cwd);
        strcpy(previousDirectory, cwd);
        strcpy(currentDirectory, cwd);
        //printf("%s\n",homeDirectory);
    }

    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        char relativePath[max_path];
        // printf("%s\n", cwd);
        get_relative_path(homeDirectory, currentDirectory, relativePath);
        printf("%s@%s:%s>", pw->pw_name, system_info.nodename , relativePath);
    } else {
        perror("getcwd() error");
    }
}