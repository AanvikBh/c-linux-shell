#ifndef __PAST2_H__
#define __PAST2_H__

void shiftLines(char lines[MAX_LINES][MAX_LINE_LENGTH], int lineCount);

char *strtrim(char *str);

bool contains_pastevents(char *input);

void adding_pastevents(int argCount, char *argString[]);

int pastevents(int argCount, char *argString[]);

#endif