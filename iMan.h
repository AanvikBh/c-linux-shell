#ifndef __IMAN_H__
#define __IMAN_H__

void extractBodyContent(char *html);

int resolveHost(char *hostname, char *ip);

int iMan(char* argString[], int argCount, int port);

#endif