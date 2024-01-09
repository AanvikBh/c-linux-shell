#ifndef __WARP_H__
#define __WARP_H__

void get_home_directory(char *home_path);

void get_parent_directory(char *path);

int warp_driver(const char *path, char *home_path) ;

int warp(char *args[], int countArgs, char *home_path) ;

#endif