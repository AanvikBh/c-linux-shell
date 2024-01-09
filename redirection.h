#ifndef __REDIRECTION_H__
#define __REDIRECTION_H__

char** redirect_driver(char* argString[], int argCount, char* untokenized_str);

int redirect(char *argString[], int argCount, char *untokenized_str);

void reset_fd();

#endif