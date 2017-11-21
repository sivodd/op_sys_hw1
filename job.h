
#ifndef SMASH_JOB_H
#define SMASH_JOB_H

#include "commands.h"
#include <time.h>
#include <string>

job L_Fg_Cmd;
job L_Bg_Cmd;
list<job> jobs; //This represents the list of jobs. Please change to a preferred type (e.g array of char*)
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
    };


};


#endif //SMASH_JOB_H
