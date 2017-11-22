//		commands.c
//********************************************
#include <iostream>
#include <errno.h>
#include <unistd.h>
#include "commands.h"
/////////////////////////////////////////////////////////////////////// need to add signals.h to recognize kill command?
using namespace std;
using std::cerr;
using std::endl;

extern job L_Fg_Cmd;
extern job L_Bg_Cmd;
extern list<job> jobs; //This represents the list of jobs.
extern char History[MAX_HISTORY][MAX_LINE_SIZE];
extern int hist_iter;
extern bool hist_flag;
extern char prev_dir[MAX_LINE_SIZE];
//********************************************
// function name: ExeCmd
// Description: interperts and executes built-in commands
// Parameters: pointer to jobs, command string
// Returns: 0 - success,1 - failure
//**************************************************************************************
int ExeCmd(list<job>& jobs, char* lineSize, char* cmdString)
{
    char* cmd;
    char* args[MAX_ARG];
    char pwd[MAX_LINE_SIZE];
	const char* delimiters = " \t\n";
    int i = 0, num_arg = 0;
    bool illegal_cmd = FALSE; // illegal command
    cmd = strtok(lineSize, delimiters);
    if (cmd == NULL)
        return 0;
    args[0] = cmd;
    for (i=1; i<MAX_ARG; i++)
    {
        args[i] = strtok(NULL, delimiters);
        if (args[i] != NULL)
            num_arg++;
    }
/*************************************************/
// Built in Commands PLEASE NOTE NOT ALL REQUIRED
// ARE IN THIS CHAIN OF IF COMMANDS. PLEASE ADD
// MORE IF STATEMENTS AS REQUIRED
/*************************************************/
    if (!strcmp(cmd, "cd") )
    {
//      checking if the number of args is correct
        if(num_arg != 1){
            illegal_cmd = TRUE;
        }
        else if(!strcmp(args[1], "-")) {
//          prev_dir[0] must be / ???
            if (prev_dir[0] == 0) {
                perror("cd - no previous dir.");
                return -1;
            }
//          cd arg is "-"
            else if (getcwd(pwd, MAX_LINE_SIZE) == NULL) {
                perror("cd - can't reach current dir");
                return -1;
            }
//          pwd updated
            else if (chdir(prev_dir) == -1) {
                cerr << "smash error: > " << args[1] << "- path not found" << endl;
                return -1;
            }
//          current working dir updated
            else {
                strcpy(prev_dir, pwd);
            }
        }
//      args[1] is not '-'
        else {
//          remembering the current dir before we switch to the new one
            if (getcwd(pwd, MAX_LINE_SIZE) == NULL) {
                perror("cd - can't reach current dir");
                return -1;
            }
//          changing working dir to args[1]
            if(chdir(args[1]) == -1){
                cerr << "smash error: > " << args[1] << "- path not found" << endl;
                return -1;
            }
//          opened new dir - copy old pwd to prev_dir
            strcpy(prev_dir, pwd);
        }
    }

/*************************************************/
    else if (!strcmp(cmd, "pwd"))
    {
//      checking if the number of args is correct
        if (num_arg != 0){
            illegal_cmd = TRUE;
        }
//      get current working dir
        else if (getcwd(pwd, MAX_LINE_SIZE)==NULL) {
            perror("pwd - can't read current directory");
            return -1;
        }
        else(printf("%s\n", pwd));
    }
/*************************************************/
    else if (!strcmp(cmd, "history"))
    {
        if (num_arg!= 0){
            illegal_cmd = TRUE;
        }
        else{
            if (hist_flag == TRUE){
                for(i=0; i<MAX_HISTORY;i++)
					if (i + hist_iter >= MAX_HISTORY) {
						printf("%s\n", History[i + hist_iter- MAX_HISTORY]);
					}
					else {
						printf("%s\n", History[i + hist_iter]);
					}
            } 
            else {
//              hist_iter didn't exceeded 50 iterations.
                for (i=0; i<hist_iter; i++)
                    printf("%s\n", History[i]);
            }
        }
    }
/*************************************************/
    else if (!strcmp(cmd, "jobs"))
    {
        if (num_arg != 0){
            illegal_cmd = TRUE;
        }
        updateJobList(jobs);
        int job_num = 1;
        for(list<job> ::iterator iter = jobs.begin(); iter != jobs.end(); iter++){///////////////////////supposed to be only not done jobs, does  it do that?
			if (iter->suspend == TRUE) {
				cout << "[" << job_num << "] " << iter->name << " : " << iter->pid << " " << difftime(time(NULL), iter->start_time) << " secs (Stopped)" << endl;
		   }
			else {
				cout << "[" << job_num << "] " << iter->name << " : " << iter->pid << " " << difftime(time(NULL), iter->start_time) << " secs" << endl;
			}	
			job_num++;
         }
    } 
/*************************************************/
    else if (!strcmp(cmd, "kill"))
    {
        if (num_arg != 2){
            illegal_cmd = TRUE;
        }
        else {
            int signum = atoi(strtok(args[1], "-"));
//          if atoi signum failed
            if (signum == 0 ){
                perror("kill - signum wasn't entered correctly");
                return -1;
            }
            int job_num = atoi(args[2]);
//          if atoi job_num fails
            if (job_num == 0){
                perror("kill - illegal job number");
                return -1;
            }
//          does job num exist
            else if (job_num < 1 || job_num > jobs.size()){
                cerr << "smash error: > kill " << job_num << " - job does not exist" << endl;
                return -1;
            }
//          job_num exists
            else {
                list<job>:: iterator iter = jobs.begin();
                advance(iter, job_num-1);
                if(kill(iter->pid, signum) != 0){
                    cerr << "smash error: > kill " << job_num << " - cannot send signal" << endl;
                    return -1;
                }
                else{
                    printf("smash > signal number %d was sent to pid %d\n", signum, iter->pid);/////////////// do we need to print "smash >"
					if (signum == SIGTSTP)
						iter->suspend = TRUE;
					if (signum == SIGCONT)
						iter->suspend = FALSE;
					updateJobList(jobs);
                }
            }

        }
    }
/*************************************************/
    else if (!strcmp(cmd, "showpid"))
    {
        if(num_arg !=0 ){
            illegal_cmd = TRUE;
        } else {
            printf("smash pid is %d\n", getpid());
        }
    }
/*************************************************/
    else if (!strcmp(cmd, "fg"))
    {
       // job fg_job;
        if((num_arg != 0) && (num_arg != 1)){
            illegal_cmd = TRUE;
        }

        else if(num_arg == 0) {
			if (jobs.size() == 0) {
				perror(" fg - no jobs in bg");
				return -1;
			}
            else {
                L_Fg_Cmd = *jobs.end();
                jobs.pop_back();
            }
        }
        else {
            int command_number = atoi(args[1]);
            if (command_number !=0 && jobs.size() <= command_number){
                list<job>:: iterator iter = jobs.begin();
                advance(iter, command_number-1);
                L_Fg_Cmd = *iter;
                jobs.erase(iter);
            } else {
                perror("fg - wrong command number");
                return -1;
            }
        }
        if (L_Fg_Cmd.suspend == TRUE) {
            if (kill(L_Fg_Cmd.pid, SIGCONT) == 0) {
                printf("smash > signal SIGCONT was sent to pid %d \n", L_Fg_Cmd.pid);
                L_Fg_Cmd.suspend = FALSE;
            } else {
                perror("fg - error in SIGCONT signal ");
                return -1;
            }
        }
		cout << L_Fg_Cmd.name << endl;
        if (waitpid(L_Fg_Cmd.pid, NULL, WUNTRACED)== -1) {//not sure about this syntax
            perror(" fg - waitpid error");
        }
		L_Fg_Cmd.pid = - 1;
    }
/*************************************************/
    else if (!strcmp(cmd, "bg"))
    {
        if((num_arg !=0) && (num_arg != 1)) {
            illegal_cmd = TRUE;
        }
        else if(num_arg == 0) {
			list<job>::iterator iter = jobs.end();
			for (; iter != jobs.begin(); iter--) {
				if (iter->suspend == true) {
					break;
				}
			}
			if (iter->suspend == false) {
				perror("no bg suspended");
				return -1;
			}
			L_Bg_Cmd = *iter;
        }

        else // num args == 1
        {
            int command_number = atoi(args[1]);
            if (command_number != 0 && jobs.size() <= command_number) {
                list<job>::iterator iter = jobs.begin();
                advance(iter, command_number - 1);
                if (iter->suspend == FALSE) {
                    cerr << "bg command number -" << command_number << "is already in bg" << endl;
                    return -1;
                } else {
                    L_Bg_Cmd = *iter;
                }
            } else {
                perror("bg - wrong command number");
                return -1;
            }
        }

        if (kill(L_Bg_Cmd.pid, SIGCONT) == 0) {
            printf("smash > signal SIGCONT was sent to pid %d \n", L_Bg_Cmd.pid);
            L_Bg_Cmd.suspend = false;
			cout << L_Bg_Cmd.name << endl;
        } else {
                perror("bg - error in SIGCONT signal ");
                return -1;
            }
    }
/*************************************************/
    else if (!strcmp(cmd, "quit"))
    {
        if((num_arg !=0) && (num_arg !=1) )
            illegal_cmd = true;
        else if (num_arg == 0){
            exit(-1);
        }
        //        num args is 1
        else {
            if (!strcmp(args[1], "kill")) {
                int job_num = 1;
                for (list<job>::iterator iter = jobs.begin(); iter != jobs.end(); iter++) {
					cout << "[" << job_num << "] " << iter->name << " - Sending SIGTERM... Done." << endl;
                    job_num++;
                    if (kill(iter->pid, SIGTERM) == 0) {
//                        job is alive
                        time_t sigTime = time(NULL);
                        while (difftime(time(NULL), sigTime) < 5) {
                            if (waitpid(iter->pid, NULL, WNOHANG) != 0)
                                break;
                        }
                        if (waitpid(iter->pid, NULL, WNOHANG) == 0) {
                            printf("(5 sec passed) Sending SIGKILL...\n");
                            if (kill(iter->pid, SIGKILL) == 0) {
                                printf("Done.\n");
                            }
                                // sigkill was not sent;
                            else {
                                perror("quit kill - SIGKILL error");
                                return -1;
                            }
                        } else { //
                            printf("Done.\n");
                        }
                    }
                        // sigterm wasn't sent
                    else {
                        perror(" quit kill - error in SIGTERM signal");
                        return -1;
                    }
                }
            } else (illegal_cmd = true);
        }
    }
/*************************************************/
    else if (!strcmp(cmd, "mv"))
    {
        if(num_arg !=2)
            illegal_cmd = true;
        else{
            string old_name = args[1];
            string new_name = args[2];
            list<job>::iterator iter2;
            for (list<job>::iterator iter1 = jobs.begin(); iter1 != jobs.end(); iter1++ ){
                if (old_name == iter1->name){
                    iter2 = iter1;
                    old_name = "ok";
                }
                if (new_name!=iter1->name){
                    new_name = "taken";
                    break;
                }
            }
            if (old_name == "ok" && new_name != "taken"){
				cout << iter2->name << " has been renamed to " << new_name << endl;
                iter2->name == new_name;
            }
            else{
                perror("mv - can't find old name or new name already exists.");
                return -1;
            }
        }
    }
/*************************************************/
    else // external command
    {
        ExeExternal(args, cmdString);
        return 0;
    }
    if (illegal_cmd == TRUE) {
        printf("smash error: > \"%s\"\n", cmdString);
        return 1;
    }
    return 0;
}
//**************************************************************************************
// function name: ExeExternal
// Description: executes external command
// Parameters: external command arguments, external command string
// Returns: void
//**************************************************************************************
void ExeExternal(char *args[MAX_ARG], char* cmdString)
{
    int pID;
    switch(pID = fork())
    {
        case -1://error, no child process is created
            // Add your code here (error)

            perror("ExeExternal - no child process was created.");
            return;
        case 0 :
            // Child Process
            // Add your code here (execute an external command)
            setpgrp();
            execvp(args[0], args);
            perror(" ExecExternal - command didn't run.");
            if (kill(pID, SIGTERM) == 0){
                printf("smash > signal SIGTERM was sent to pid %d\n", pID);
            } else perror("ExecExternal - error in sending SIGTERM signal.");
            return;

        default: // parent
            // Add your code here
            L_Fg_Cmd.pid = pID;
            L_Fg_Cmd.name = args[0];
            L_Fg_Cmd.suspend = false;
            L_Fg_Cmd.start_time = time(NULL);
			if (waitpid(pID, NULL, WUNTRACED) == -1) {
				perror("ExecExternal: waitpid failed!!!\n");
			}
			L_Fg_Cmd.pid = -1;
			L_Fg_Cmd.name = "\0";
            return;
    }
}
//**************************************************************************************
// function name: ExeComp
// Description: executes complicated command
// Parameters: command string
// Returns: 0- if complicated -1- if not
//**************************************************************************************
int ExeComp(char* lineSize)
{
    char ExtCmd[MAX_LINE_SIZE+2];
    char *args[MAX_ARG];
	if ((strstr(lineSize, "|")) || (strstr(lineSize, "<")) || (strstr(lineSize, ">"))
		|| (strstr(lineSize, "*")) || (strstr(lineSize, "?")) || (strstr(lineSize, ">>"))
		|| (strstr(lineSize, "|&")  && lineSize[strlen(lineSize) - 2] != '&')) {
		args[0] = (char*)"sh";
		args[1] = (char*)"-f";
		args[2] = (char*)"-c";
		args[3] = lineSize;
		for (int i = 4; i<MAX_ARG; i++)
		{
			args[i] = NULL;
		}
		int pID;
		switch (pID = fork()) {
		case -1://error, no child process is created
				// Add your code here (error)

			perror("ExeComp - no child process was created.");
			return 0;
		case 0:
			// Child Process
			// Add your code here (execute an external command)
			setpgrp();
			execvp(args[0], args);
			perror(" ExeComp - command didn't run.");
			if (kill(pID, SIGTERM) == 0) {
				printf("smash > signal SIGTERM was sent to pid %d\n", pID);
			}
			else perror("ExeComp - error in sending SIGTERM signal.");
			return 0;

		default: // parent
				 // Add your code here
			L_Fg_Cmd.pid = pID;
			L_Fg_Cmd.name = args[0];
			L_Fg_Cmd.suspend = false;
			L_Fg_Cmd.start_time = time(NULL);
			if (waitpid(pID, NULL, WUNTRACED) == -1) {
				perror("ExeComp: waitpid failed!!!\n");
			}
			L_Fg_Cmd.pid = -1;
			L_Fg_Cmd.name = "\0";
			return 0;
		}
	}
    return -1;
}
//**************************************************************************************
// function name: BgCmd
// Description: if command is in background, insert the command to jobs
// Parameters: command string, pointer to jobs
// Returns: 0- BG command -1- if not
//**************************************************************************************
int BgCmd(char* lineSize, list<job>& jobs) {
	//  updateJobList();
	const char* delimiters = " \t\n";
	char *args[MAX_ARG];

	if (lineSize[strlen(lineSize) - 2] == '&')
	{
		lineSize[strlen(lineSize) - 2] = '\0';
		// Add your code here (execute a in the background)

		if ((strstr(lineSize, "|")) || (strstr(lineSize, "<")) || (strstr(lineSize, ">"))
			|| (strstr(lineSize, "*")) || (strstr(lineSize, "?")) || (strstr(lineSize, ">>"))
			|| (strstr(lineSize, "|&")))
		{
			args[0] = (char*)"sh";
			args[1] = (char*)"-f";
			args[2] = (char*)"-c";
			args[3] = lineSize;

		}
		else { // not complicated - separate command into args
			args[0] = strtok(lineSize, delimiters);
			if (args[0] == NULL)
				return 0;

			for (int i = 1; i < MAX_ARG; i++) {
				args[i] = strtok(NULL, delimiters);
			}
		}

		int pID;
		switch (pID = fork())
		{
		case -1:
			// error
			perror("BgCmd - error in fork");
			return -1;
		case 0:
			// Child Process (execute an external command)
			setpgrp();// each chlid should execute, it changes the group id.
			execvp(args[0], args);
			if (kill(pID, SIGTERM) == 0) {
				printf("smash > signal SIGTERM was sent to pid %d\n", pID);
			}
			else perror("BgCmd - error in sending SIGTERM signal");
			return -1;

		default: //father
			job new_job = job(args[0], pID, time(NULL), false);
			jobs.push_back(new_job);
			return 0;
		}
	}

    return -1; //command is not external& and not complicated&
}

//**************************************************************************************
// function name: updateJobList
// Description:
// Parameters:
// Returns:
//**************************************************************************************
void updateJobList(list<job> & jobs){

    for(list<job>:: iterator iter = jobs.begin(); iter != jobs.end(); iter++){
        if(waitpid(iter->pid, 0, WNOHANG) != 0){
            jobs.erase(iter);
        }
    }
}
