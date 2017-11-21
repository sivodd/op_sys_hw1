#ifndef _COMMANDS_H
#define _COMMANDS_H
#include <unistd.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <iostream>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <list>
//include "smash.cpp"
#include "job.h"


#define MAX_LINE_SIZE 80
#define MAX_ARG 20

typedef enum {FALSE, TRUE} BOOL;

int ExeComp(char* lineSize);
int BgCmd(char* lineSize, list<job>& jobs);
int ExeCmd(list<job> &jobs, char* lineSize, char* cmdString);
void ExeExternal(char *args[MAX_ARG], char* cmdString);

void updateJobList(list<job>&);

#endif

