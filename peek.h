#ifndef __PEEK_H__
#define __PEEK_H__

// Function prototypes
int compareFileInfo(const void *a, const void *b);

char* getMonth(int month);

char* permissions(mode_t mode);

char* getGroup(struct stat entry_stat);

char* getOwner(struct stat entry_stat);

void list_files_and_directories(const char *path, int show_hidden, int show_details);

int peek(int argCount, char *argString[]) ;


#endif