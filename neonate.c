#include "headers.h"

void die(const char *s)
{
    perror(s);
    exit(1);
}

struct termios orig_termios;

void disableRawMode()
{
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios) == -1)
        die("tcsetattr");
}

/**
 * Enable row mode for the terminal
 * The ECHO feature causes each key you type to be printed to the terminal, so you can see what you’re typing.
 * Terminal attributes can be read into a termios struct by tcgetattr().
 * After modifying them, you can then apply them to the terminal using tcsetattr().
 * The TCSAFLUSH argument specifies when to apply the change: in this case, it waits for all pending output to be written to the terminal, and also discards any input that hasn’t been read.
 * The c_lflag field is for “local flags”
 */
void enableRawMode()
{
    if (tcgetattr(STDIN_FILENO, &orig_termios) == -1)
        die("tcgetattr");
    atexit(disableRawMode);
    struct termios raw = orig_termios;
    raw.c_lflag &= ~(ICANON | ECHO);
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1)
        die("tcsetattr");
}

pid_t findLatestProcess()
{
    pid_t latestPID = -1;

    FILE *file = fopen("/proc/loadavg", "r");
    if (file) {
        char buffer[4096];
        ssize_t bytesRead = fread(buffer, 1, sizeof(buffer) - 1, file);
        if (bytesRead > 0) {
            buffer[bytesRead] = '\0'; // Null-terminate the buffer

            // Tokenize the content using spaces
            char *token = strtok(buffer, " ");
            for (int i = 0; i < 4 && token; i++) {
                token = strtok(NULL, " "); // Move to the next token
            }

            // If there's a token and it ends with a newline, remove it
            if (token && token[strlen(token) - 1] == '\n') {
                token[strlen(token) - 1] = '\0';
            }

            // Convert the token to an integer (if it's not NULL)
            if (token) {
                latestPID = atoi(token);
            }
        }
        fclose(file);
    }

    return latestPID;
}

int neonate(int argCount, char *argString[])
{
    if (argCount != 3)
    {
        fprintf(stderr, "Usage: %s -n <time_in_seconds>\n", argString[0]);
        return 1;
    }

    int timeGap = atoi(argString[2]);
    char timeChar=argString[2][0];
    if (timeGap <= 0 || !isdigit(timeChar) || timeGap > 10)
    {
        printf("Invalid time value\n");
        return 1;
    }

    pid_t neoChild;
    neoChild = fork();
    if (neoChild == -1)
    {
        perror("Fork:");
        return 1;
    }
    char c;
    enableRawMode();
    if (neoChild == 0)
    {
        // Child process

        while (1)
        {
            fflush(stdout);
            fflush(stdin);
            sleep(timeGap); // Wait for the specified time

            pid_t latestPID = findLatestProcess();
            // int readChar = read(STDIN_FILENO, &c, sizeof(char));
            // if (readChar == 1)
            // {
            //     if (c == 'x' || c == 'X')
            //     {
            //         kill(neoChild, SIGKILL);
            //         return 0;
            //     }
            // }
            // if (latestProcessPID != -1) {
            //     printf("Latest Process PID after %d seconds: %d\n", timeGap, latestProcessPID);
            // } else {
            //     printf("No processes found after %d seconds\n", timeGap);
            // }
            if (latestPID != -1)
            {
                printf("%d\n", latestPID);
            }
            else
            {
                printf("No process found\n");
                break;
            }
        }
    }
    else
    {
        // Parent process
        int readChar;
        while((readChar = read(STDIN_FILENO, &c, sizeof(char))) == 1)
        {
            if (c == 'x' || c == 'X')
            {
                kill(neoChild, SIGKILL);
                disableRawMode();
                return 0;
            }
        }
        disableRawMode();
        return 0;
    }
}
