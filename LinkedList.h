#ifndef __LL_H__
#define __LL_H__

PtrToNode MakeNode(char* command[], pid_t currPID, int countArgs);

char* concatenateArgs(char* args[], int argc);

List CreateEmptyList();

void InsertAtFront(List L, char* X[], pid_t new, int countArgs);

void DeleteNode(List L, PtrToNode P);

void PrintList(List L);

void hasProcessTerminated(List L);

int comparatorProc(const void* a, const void* b);

void activityCheck(List L);


#endif