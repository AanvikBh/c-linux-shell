#ifndef __HEADERS_H__
#define __HEADERS_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <pwd.h>
#include <ctype.h>
#include <signal.h>
#include <sys/wait.h>
#include <time.h>
#include <sys/utsname.h>
#include <dirent.h>
#include <grp.h>
#include <stdbool.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <termios.h>

#define max_path 1024
#define MAX_INPUT_LENGTH 1024
#define MAX_COMMANDS 64
#define MAX_ARGS 64
#define MAX_PATH_LEN 1024
#define MAX_DIR_FILES 10000
#define MAX_LINES 15
#define MAX_LINE_LENGTH 1000
#define MAX_TOKENS 10
#define max_name_len 32
#define MAX_BG_PROC 128
#define MAX_PIPE_COMMANDS 20
#define MAX_COMMAND_ARGS 32

#ifndef PATH_MAX
#define PATH_MAX 1024
#endif

#define RUNNING 1
#define STOPPED 0


struct Node {
    char commandName[max_name_len];
    char completeCommand[MAX_INPUT_LENGTH];
    int bgState;
    pid_t pid;
    struct Node* next;
};

typedef struct Node* List;
typedef struct Node* PtrToNode;

#include "execute.h"
#include "LinkedList.h"
#include "pastevents.h"
#include "pastevents2.h"
#include "peek.h"
#include "proclore.h"
#include "prompt.h"
#include "seek.h"
#include "tokenize.h"
#include "warp.h"

#include "ping.h"
#include "activities.h"
#include "redirection.h"
#include "fg.h"
#include "pipe.h"
#include "neonate.h"
#include "iMan.h"

#endif