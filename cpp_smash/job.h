
#ifndef SMASH_JOB_H
#define SMASH_JOB_H

#include <time.h>
#include <string>
#include <list>

/* #include "commands.h" */

using namespace std;

class job {

	public:
    string name;
    int pid;
    time_t start_time;
    bool suspend;


	job();
	job(string name, int pid, time_t start, bool suspend);

};

#define MAX_LINE_SIZE 80
#define MAXARGS 20
#define MAX_JOB_NUMBER 100
#define MAX_HISTORY 50


#endif //SMASH_JOB_H
