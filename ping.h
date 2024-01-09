#ifndef __PING_H__
#define __PING_H__

int ping(int argCount, char* argString[]);

void ctrlC_handler(int signum);

void ctrlZ_handler(int signum);

void ctrlD_handler(int signum);

#endif