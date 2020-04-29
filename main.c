#define _GNU_SOURCE
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sched.h>
#include <errno.h>
#include <unistd.h>
#include "process.h"
#include "scheduler.h"

int get_policy( char* policy_string){
	if( strcmp( policy_string, "FIFO" ) == 0 ) return FIFO;
	if( strcmp( policy_string, "RR" ) == 0 ) return RR;
	if( strcmp( policy_string, "SJF" ) == 0 ) return SJF;
	if( strcmp( policy_string, "PSJF" ) == 0 ) return PSJF;

	fprintf(stderr, "Invalid policy: %s\n", policy_string);
	return -1;
}

int main( int argc, char* argv[]){
	char policy_string[20];
	scanf("%s", policy_string);

	int policy = -1;
	if( (policy = get_policy(policy_string)) < 0 ){
		fprintf(stderr, "Invalid policy: %s\n", policy_string);
		return 0;
	}


	int N = 0;
	scanf("%d", &N);

	struct process *proc;
	proc = (struct process *)malloc( N * sizeof(struct process));
	for (int i = 0; i < N; i++){
		scanf("%s%d%d", proc[i].name, &proc[i].t_ready, &proc[i].t_exec);
		proc[i].pid = -1;
	}

	scheduling( proc, N, policy);

	return 0;
}


