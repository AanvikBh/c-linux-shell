#include "headers.h"

extern char homeDirectory[max_path];
extern pid_t terminal_pid;

void proclore(int countArgs, char* argString[]) {
    if (countArgs > 2) {
        printf("Incorrect usage of proclore\n");
        return;
    }

    char path[1024];
    char pid[10];

    pid_t pid_num = countArgs == 2 ? atoi(argString[1]) : terminal_pid;

    snprintf(pid, sizeof(pid), "%d", pid_num);

    snprintf(path, sizeof(path), "/proc/%s/status", pid);

    FILE* file = fopen(path, "r");
    if (file == NULL) {
        perror("Failed to open file");
        return;
    }

    int is_fg = 1;

    char line[256];
    char processStatus[10] = "";
    char virtualMemory[50] = "";
    char executablePath[256] = "";

    if (countArgs == 2 && terminal_pid != atoi(argString[1])) {
        is_fg = 0;
    }

    while (fgets(line, sizeof(line), file)) {
        if (strncmp(line, "State:", 6) == 0) {
            char* token = strtok(line, " \t");
            if (token) {
                token = strtok(NULL, " \t");
                if (token) {
                    strcpy(processStatus, token);
                }
            }
        } else if (strncmp(line, "VmSize:", 7) == 0) {
            char* token = strtok(line, " \t");
            if (token) {
                token = strtok(NULL, " \t");
                if (token) {
                    strcpy(virtualMemory, token);
                }
            }
        }
    }

    fclose(file);

    // Get the process group using getpgid
    pid_t process_group = getpgid(pid_num);

    // Get the executable path
    snprintf(path, sizeof(path), "/proc/%s/exe", pid);
    ssize_t len = readlink(path, executablePath, sizeof(executablePath) - 1);
    if (len != -1) {
        executablePath[len] = '\0';

        // Replace homeDirectory with ~ in executablePath
    char *homeSubstr = strstr(executablePath, homeDirectory);
    if (homeSubstr == executablePath) {
        char newExecutablePath[sizeof(executablePath) + 1];
        snprintf(newExecutablePath, sizeof(newExecutablePath), "~%s", executablePath + strlen(homeDirectory));
        strcpy(executablePath, newExecutablePath);
    }
    } else {
        perror("Failed to read executable path");
    }

    if (is_fg) {
        strcat(processStatus, "+");
    }

    printf("PID: %d\n", pid_num);
    printf("Process Status: %s\n", processStatus);
    printf("Process Group: %d\n", process_group); // Print the process group
    printf("Virtual Memory: %s\n", virtualMemory);
    printf("Executable Path: %s\n", executablePath);
}
