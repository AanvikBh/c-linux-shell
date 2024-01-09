#include "headers.h"

extern List backgroundlist;

int activities(char* args[], int argCount) {
    activityCheck(backgroundlist);
    return 0;
}