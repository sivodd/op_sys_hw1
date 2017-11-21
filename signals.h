#ifndef _SIGS_H
#define _SIGS_H
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include "job.h"
#include <list>
#include <iostream>

void signal_int_fg(int Signal_number);

void signal_stop_fg(int Signal_number);




#endif
