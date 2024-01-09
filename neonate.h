#ifndef __NEONATE_H__
#define __NEONATE_H__

void die(const char *s);

void disableRawMode();

void enableRawMode();

pid_t findLatestProcess();

int neonate(int argCount, char *argString[]);

#endif