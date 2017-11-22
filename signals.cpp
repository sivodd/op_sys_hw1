// signals.c
// contains signal handler funtions
// contains the function/s that set the signal handlers

/*******************************************/
/* Name: handler_cntlc
   Synopsis: handle the Control-C */
#include "signals.h"


extern job L_Fg_Cmd;
extern job L_Bg_Cmd;
extern list<job> jobs; //This represents the list of jobs.


   //**************************************************************************************
// function name: signal_int_fg
// Description: send SIGINT signal to the process in the foreground
// Parameters: the job
// Returns: None
//**************************************************************************************
void signal_int_fg(int Signal_number)
{
	if (L_Fg_Cmd.pid != -1) {
		if (kill(L_Fg_Cmd.pid, SIGTSTP) == 0) {
			cout << "smash > signal SIGTSTP was sent to pid " << L_Fg_Cmd.pid << endl;
		
		}
		else
			perror("smash error: > can't send signal\n");
	}
	L_Fg_Cmd.pid = -1;
	L_Fg_Cmd.name = "\0";
}

//**************************************************************************************
// function name: signal_stop_bg
// Description: send SIGTSTP signal to the process in the foreground, and insert the process to job list
// Parameters: the job and jobs list
// Returns: None
//**************************************************************************************
void signal_stop_fg(int Signal_number)
{
	if (L_Fg_Cmd.pid != -1) {
		if (kill(L_Fg_Cmd.pid, SIGTSTP) == 0) {
				cout << "smash > signal SIGTSTP was sent to pid " << L_Fg_Cmd.pid << endl;
		}
		else
			perror("smash error: > can't send signal\n");
	}
	L_Fg_Cmd.suspend = true;
	jobs.push_back(L_Fg_Cmd);
	L_Fg_Cmd.pid = -1;
	L_Fg_Cmd.name = "\0";
}

