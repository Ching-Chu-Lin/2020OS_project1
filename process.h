#ifndef _PROCESS_H_
#define _PROCESS_H_

#include <sys/types.h>

#define UNIT_T(x){ for(int i = 0; i < x ; i++){ volatile unsigned long j; for( j = 0; j < 1000000UL; j++);}}

struct process {
	char name[32];
	int t_ready;
	int t_exec;
	pid_t pid;
};

int proc_assign_cpu(int pid, int core);
int proc_setscheduler(int pid, int SCHED_var);
int proc_exec(struct process proc);



#endif
