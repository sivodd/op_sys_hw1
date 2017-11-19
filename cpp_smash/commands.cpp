//		commands.c
//********************************************
#include "commands.h"
#include "job.h"
#include <iostream>
#include <errno.h>
using namespace std;
using std::cerr;
using std::endl;

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
    string delimiters = " \t\n";
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
                perror("");
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
                for(i=0; i<50;i++)
                    printf("%s\n", History[i+hist_iter]);
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
//        update job list();
        int job_num = 1;
        for(list<job> ::iterator iter = jobs.begin(); iter != jobs.end(); iter++){
            printf("[%d] %s : %d %f secs\n", job_num, iter->name, iter->pid,difftime(time(NULL), iter->start_time));
            job_num++;
                }
    } // remember stopped
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
                    printf("signal number %d was sent to pid %d\n", signum, iter->pid);
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
        job fg_job;
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
                printf("SIGCONT was sent to pid %d \n", L_Fg_Cmd.pid);
                L_Fg_Cmd.suspend = FALSE;
            } else {
                perror("fg - error in SIGCONT signal ");
                return -1;
            }
        }
        printf("%s\n", L_Fg_Cmd.name);
        if (waitpid(L_Fg_Cmd.pid, NULL, WUNTRACED)== -1) {//not sure about this syntax
            perror(" fg - waitpid error");
            return -1;
        }
    }
/*************************************************/
    else if (!strcmp(cmd, "bg"))
    {
        if((num_arg !=0) && (num_arg != 1)) {
            illegal_cmd = TRUE;
        }
        else if(num_arg == 0) {
//            add last suspended to job list
//            printf("%s\n", job name);
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
            printf("SIGCONT was sent to pid %d \n", L_Bg_Cmd.pid);
            L_Bg_Cmd.suspend = FALSE;
            printf("%s\n", L_Bg_Cmd.name);
        } else {
                perror("bg - error in SIGCONT signal ");
                return -1;
            }
    }
/*************************************************/
    else if (!strcmp(cmd, "quit"))
    {
        if((num_arg !=0) && (num_arg !=1) )
            illegal_cmd = TRUE;
        else if (num_arg == 0){
            exit(-1);
        }
        //        num args is 1
        else {
            if (!strcmp(args[1], "kill")) {
                int job_num = 1;
                for (list<job>::iterator iter = jobs.begin(); iter != jobs.end(); iter++) {
                    printf("[%d] %s - Sending SIGTERM... Done.\n", job_num, iter->name);
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
                        } else {
                            printf("Done.\n");
                        }
                    }
                        // sigterm wasn't sent
                    else {
                        perror(" quit kill - error in SIGCONT signal");
                        return -1;
                    }
                }
            } else (illegal_cmd = TRUE);
        }
    }
/*************************************************/
    else if (!strcmp(cmd, "mv"))
    {
        if(num_arg !=2)
            illegal_cmd = TRUE;
        else{
            string old_name = args[1];
            string new_name = args[2];
            list<job>::iterator iter2;
            for (list<job>::iterator iter1 = jobs.begin(); iter1 != jobs.end(); iter1++ ){
                if (!strcmp(old_name, iter1->name)){
                    iter2 = iter1;
                    old_name = "ok";
                }
                if (!strcmp((new_name,iter1->name))){
                    new_name = "taken";
                    break;
                }
            }
            if (old_name == "ok" && new_name != "taken"){
                printf("%s has been renamed to %s\n", iter2->name, new_name);
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
        case -1:
            // Add your code here (error)

            /*
            your code
            */
        case 0 :
            // Child Process
            setpgrp();

            // Add your code here (execute an external command)

            /*
            your code
            */

        default:
            // Add your code here

            /*
            your code
            */
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
    if ((strstr(lineSize, "|")) || (strstr(lineSize, "<")) || (strstr(lineSize, ">")) || (strstr(lineSize, "*")) || (strstr(lineSize, "?")) || (strstr(lineSize, ">>")) || (strstr(lineSize, "|&")))
    {
        // Add your code here (execute a complicated command)

        /*
        your code
        */
    }
    return -1;
}
//**************************************************************************************
// function name: BgCmd
// Description: if command is in background, insert the command to jobs
// Parameters: command string, pointer to jobs
// Returns: 0- BG command -1- if not
//**************************************************************************************
int BgCmd(char* lineSize, void* jobs)
{

    char* Command;
    char* delimiters = " \t\n";
    char *args[MAX_ARG];
    if (lineSize[strlen(lineSize)-2] == '&')
    {
        lineSize[strlen(lineSize)-2] = '\0';
        // Add your code here (execute a in the background)

        /*
        your code
        */

    }
    return -1;
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
