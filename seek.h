#ifndef __SEEK_H__
#define __SEEK_H__

void get_relative_path2(const char *initial, const char *current, char *relative_path);

void print_colored(char *path, int is_directory, char* home_path);

void seek_file(char *current_dir, char *target_file, int *numFiles, int *numDirectories, char* uniquePathFile) ;

void seek_dir(char *current_dir, char *target_dir, int *numDirectories, char* uniquePathDirectory);

int seek(int argCount, char* argString[]);

#endif