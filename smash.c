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
#include "commands.h"
#include "signals.h"
#define MAX_LINE_SIZE 80
#define MAXARGS 20
#define MAX_HISTORY 50



char* L_Fg_Cmd;
void* jobs = NULL; //This represents the list of jobs. Please change to a preferred type (e.g array of char*)
char lineSize[MAX_LINE_SIZE]; 
//**************************************************************************************
// function name: main
// Description: main function of smash. get command from user and calls command functions
//**************************************************************************************
int main(int argc, char *argv[])
{
    char cmdString[MAX_LINE_SIZE]; 	   
	char history_log[MAX_HISTORY][MAX_LINE_SIZE];
	for (int i = 0; i < MAX_HISTORY; i++) {
		strcpy(history_log[i], '\0');//cleaning history log
	}
	int curr_history_slot = 0;
	char prev_dir[MAX_LINE_SIZE];
	getcwd(prev_dir, MAX_LINE_SIZE);////////////////////////////////////////////////////////////////////////need to cheack for faliure
	
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


	
	L_Fg_Cmd =(char*)malloc(sizeof(char)*(MAX_LINE_SIZE+1));
	if (L_Fg_Cmd == NULL) 
			exit (-1); 
	L_Fg_Cmd[0] = '\0';
	
    	while (1)
    	{
	 	printf("smash > ");
		fgets(lineSize, MAX_LINE_SIZE, stdin);
		strcpy(cmdString, lineSize);    
		cmdString[strlen(lineSize)-1]='\0';
		strcpy(history_log[curr_history_slot], cmdString);//updating his with new comm
		curr_history_slot++;//updating history indicator to a new empty slot 
		if (curr_history_slot == MAX_HISTORY) {//cyclic indicator. if reach max, return to zero
			curr_history_slot = 0;
		}
					// perform a complicated Command
		if(!ExeComp(lineSize)) continue; 
					// background command	
	 	if(!BgCmd(lineSize, jobs)) continue; 
					// built in commands
		ExeCmd(jobs, lineSize, cmdString, history_log , curr_history_slot , prev_dir);
		
		/* initialize for next line read*/
		lineSize[0]='\0';
		cmdString[0]='\0';
	}
    return 0;
}

