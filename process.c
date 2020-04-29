#define _GNU_SOURCE
#include "process.h"
#include <time.h>
#include <sched.h>
#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/syscall.h>
#include <linux/kernel.h>

int proc_assign_cpu(int pid, int core){
	if (core > sizeof(cpu_set_t)) {
		fprintf(stderr, "Core index error.");
		return -1;
	}

	cpu_set_t mask;
	CPU_ZERO(&mask); //clear mask
	CPU_SET(core, &mask);
		
	if (sched_setaffinity(pid, sizeof(mask), &mask) < 0) {
		perror("sched_setaffinity");
		exit(1);
	}
	return 0;
}

int proc_setscheduler(int pid, int SCHED_var){
	struct sched_param param;
	param.sched_priority = 0; //param->sched_priority must be 0 in SCHED_OTHER 

	if( sched_setscheduler(pid, SCHED_var, &param) < 0) {
		perror("sched_setscheduler");
		return -1;
	}
	return 0;
}


int proc_exec(struct process proc){

	int pid = fork();
	if (pid < 0){ perror("fork"); return -1;}

	if (pid == 0) { //child process
		struct timespec start_time, end_time;
		char to_dmesg[256];

		//if( clock_gettime( CLOCK_REALTIME, &start_time ) == -1 ){
		//	char err[256];
		//	sprintf( err, "name:%s pid:%d clock gettime, start time", proc.name, proc.pid);
		//	perror(err); exit(1);
    		//}

		syscall(334, &start_time);
		UNIT_T(proc.t_exec);
		syscall(334, &end_time);

		//if( clock_gettime( CLOCK_REALTIME, &end_time ) == -1 ){
		//	char err[256];
		//	sprintf( err, "name:%s pid:%d clock gettime, end time", proc.name, proc.pid);
		//	perror(err); exit(1);
    		//}

		sprintf( to_dmesg, "[Project1] %d %lu.%09lu %lu.%09lu\n", getpid(), start_time.tv_sec, start_time.tv_nsec, end_time.tv_sec, end_time.tv_nsec);
		syscall(333, to_dmesg);

		exit(0); //child must exit in here
	}

	//parent process	
	proc_assign_cpu(pid, 1); //child always run on cpu 1

	return pid;
}

