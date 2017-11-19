/*	smash.c
main file. This file contains the main function of smash
*******************************************************************/
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <list>
#include "commands.h"

#include "signals.h"
#define MAX_LINE_SIZE 80
#define MAXARGS 20
#define MAX_JOB_NUMBER 100

using std::list;


job L_Fg_Cmd;
job L_Bg_Cmd;
// not sure if its job* or job
//job jobs[MAX_JOB_NUMBER];

list<job> jobs; //This represents the list of jobs. Please change to a preferred type (e.g array of char*)

char lineSize[MAX_LINE_SIZE];

char History[50][MAX_LINE_SIZE];
int hist_iter;
bool hist_flag;
char prev_dir[MAX_LINE_SIZE] = {0};
//**************************************************************************************
// function name: main
// Description: main function of smash. get command from user and calls command functions
//**************************************************************************************
int main(int argc, char *argv[])
{
    char cmdString[MAX_LINE_SIZE];


    //signal declaretions
    //NOTE: the signal handlers and the function/s that sets the handler should be found in siganls.c
    /* add your code here */

    /************************************/
    //NOTE: the signal handlers and the function/s that sets the handler should be found in siganls.c
    //set your signal handlers here
    /* add your code here */

    /************************************/

    /************************************/
    // Init globals
    History[50] = {0}; //is this the right way to assign? for(i...) { Hisroty[i]='\n'}
    hist_iter = 0;
    hist_flag = FALSE;


    while (1) {
        printf("smash > ");
        fgets(lineSize, MAX_LINE_SIZE, stdin);
        strcpy(cmdString, lineSize);
        cmdString[strlen(lineSize) - 1] = '\0'; // why do we need cmdstring? do we need this after every strcpy?
        if (hist_iter == 50){
            hist_flag = TRUE;
            hist_iter = 0;
        }
        strcpy(History[hist_iter], cmdString);
        hist_iter++;
        // perform a complicated Command
        if (!ExeComp(lineSize)) continue;
        // background command
        if (!BgCmd(lineSize, jobs)) continue;
        // built in commands
        ExeCmd(jobs, lineSize, cmdString);

        /* initialize for next line read*/
        lineSize[0] = '\0';
        cmdString[0] = '\0';
    }
    return 0;
}



