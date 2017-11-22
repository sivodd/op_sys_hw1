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

#include "signals.h"

#include "commands.h"

using std::list;

extern list<job> jobs;
extern job L_Fg_Cmd;
extern job L_Bg_Cmd;

extern char cmdString[MAX_LINE_SIZE];
extern char lineSize[MAX_LINE_SIZE];
extern char History[MAX_HISTORY][MAX_LINE_SIZE];
extern int hist_iter;
extern char prev_dir[MAX_LINE_SIZE];



// not sure if its job* or job
//job jobs[MAX_JOB_NUMBER];



//**************************************************************************************
// function name: main
// Description: main function of smash. get command from user and calls command functions
//**************************************************************************************
int main(int argc, char *argv[])
{
	for (int i = 0; i < MAX_HISTORY; i++) {
		strcpy(History[i], "\0");//cleaning history log
	}
	getcwd(prev_dir, MAX_LINE_SIZE);

    //signal declaretions
    //NOTE: the signal handlers and the function/s that sets the handler should be found in siganls.c
    /* add your code here */
	signal(SIGINT, &signal_int_fg);
	signal(SIGTSTP, &signal_stop_fg);
    /************************************/
    //NOTE: the signal handlers and the function/s that sets the handler should be found in siganls.c
    //set your signal handlers here
    /* add your code here */

    /************************************/

    /************************************/
    // Init globals
    
    hist_iter = 0;
	

    while (1) {
		updateJobList(jobs);
        printf("smash > ");
        fgets(lineSize, MAX_LINE_SIZE, stdin);
        strcpy(cmdString, lineSize);
		cmdString[strlen(lineSize) - 1] = '\0'; // why do we need cmdstring? do we need this after every strcpy?
		strcpy(History[hist_iter], cmdString);//updating his with new comm
		hist_iter++;//updating history indicator to a new empty slot 
       
        if (hist_iter == MAX_HISTORY){
            hist_iter = 0;
        }

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
