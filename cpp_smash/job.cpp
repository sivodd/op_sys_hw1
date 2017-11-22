#include "job.h"

job L_Fg_Cmd;
job L_Bg_Cmd;
list<job> jobs; //This represents the list of jobs.

char cmdString[MAX_LINE_SIZE];
char lineSize[MAX_LINE_SIZE];
char History[MAX_HISTORY][MAX_LINE_SIZE];
int hist_iter = 0;
bool hist_flag;
char prev_dir[MAX_LINE_SIZE] = { 0 };



job::job() {
	name = "";
	pid = -1;
	start_time = time(NULL);
	suspend = false;
}
job::job(string name, int pid, time_t start, bool suspend) {
	this->name = name;
	this->pid = pid;
	this->start_time = start;
	this->suspend = suspend;

}

