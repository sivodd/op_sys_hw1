
#ifndef SMASH_JOB_H
#define SMASH_JOB_H

#include "commands.h"
#include <time.h>
#include <string>
#include "job.h"
#include <list>

using namespace std;

class job {

public:
    string name;
    int pid;
    time_t start_time;
    bool suspend;


    job(){
        name = "";
        pid = -1;
        start_time = time(NULL);
        suspend = false;
    }
	job(string name, int pid, time_t start, bool suspend) {
		this->name = name;
		this->pid = pid;
		this->start_time = start;
		this->suspend = suspend;

	}

};

job L_Fg_Cmd;
job L_Bg_Cmd;
list<job> jobs; //This represents the list of jobs.
#define MAX_LINE_SIZE 80
#define MAXARGS 20
#define MAX_JOB_NUMBER 100
#define MAX_HISTORY 50
char cmdString[MAX_LINE_SIZE];
char lineSize[MAX_LINE_SIZE];
char History[MAX_HISTORY][MAX_LINE_SIZE];
int hist_iter = 0;

bool hist_flag;
char prev_dir[MAX_LINE_SIZE] = { 0 };


#endif //SMASH_JOB_H
