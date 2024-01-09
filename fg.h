#ifndef __FG_H__
#define __FG_H__

void bg(char* argString[], int argCount);

void fg(char* argString[], int argCount);

void switch_bg_to_fg(int signum);

#endif