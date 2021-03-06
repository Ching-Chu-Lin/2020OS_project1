#define _GNU_SOURCE
#include "process.h"
#include "scheduler.h"
#include <stdlib.h>
#include <signal.h>
#include <stdio.h>
#include <sched.h>
#include <unistd.h>
#include <sys/wait.h>

//var managed by scheduler initializing
static int ntime = 0; // Current time in unit
static int last_run = -1;
static int running = -1; // Index of running process. -1 if no process running
static int finish_cnt = 0;
static int rr_time_cumulate = 0;
static int RR_query[20];

void _print_proc( struct process *proc, int N){
	for( int i = 0 ; i < N ; i++)
		printf("%s %d %d %d\n", proc[i].name, proc[i].t_ready, proc[i].t_exec, proc[i].pid);
	return;
}

void _print_RR_query( int N){
	printf("RR_query: ");
	for( int i = 0 ; i < N ; i++)
		printf("%d ", RR_query[i]);
	printf("\n");
	return;
}

int cmp( const void * a, const void *b){
	return ((struct process*)a)->t_ready - ((struct process*)b)->t_ready;
}



int next_process( struct process *proc, int N, int policy){
	/* return the index of next process */

	if( policy == FIFO ){
		if( running != -1 ) return running; //non-preemptive

		//proc is sorted by ready time
		for( int i = 0 ; i < N ; i++){ 
			if( proc[i].pid != -1 ) return i; //ready and has job
		}
		return -1; //no next, all job done
	}

	else if( policy == RR ){
		if( running != -1 ) RR_query[running] = ntime;
		
		int min_query_index = -1;
		for( int i = 0 ; i < N ; i++){
			if( proc[i].pid == -1 ) continue;
			
			if( min_query_index == -1 ) min_query_index = i;
			else if( RR_query[i] < RR_query[min_query_index] ) min_query_index = i;
		}
		return min_query_index;
		
	}

	else{ //SJF and PSJF

		if( policy == SJF ){
			if( running != -1 ) return running; //non-preemptive
		}
	
		int min_idx = -1;
		for( int i = 0 ; i < N ; i++){
			if( proc[i].pid == -1 ) continue;

			if( min_idx == -1 ) min_idx = i;
			else if( proc[i].t_exec < proc[min_idx].t_exec ) min_idx = i; 
		}

		if( min_idx == -1 ) return -1; //no job

		if( running != -1 && proc[min_idx].t_exec == proc[running].t_exec ) return running;
		else return min_idx; //less context switch
	}
	
}


int scheduling( struct process *proc, int N, int policy){
	//proc managed by scheduler
	//proc[i].pid == -1 : not ready or job done(t_exec == 0)

	qsort( proc, N, sizeof( struct process), cmp);

	//initialize
	for( int i = 0 ; i < N ; i++)
		RR_query[i] = -1;

	if( proc_assign_cpu(getpid(), 0) < 0 ) return -1; //set scehuler to cpu 0
	if( proc_setscheduler(getpid(), SCHED_OTHER) < 0 ) return -1;

	while( finish_cnt < N){
#ifdef DDEBUG
		if( ntime % 100 == 0 ){
			printf("--------------------------------\n");
			printf("ntime:%d running:%d last_run:%d\n", ntime, running, last_run);
			if( running != -1 ) printf("RR_query from:%d\n", RR_query[running]);
			_print_proc( proc, N); //for debug
			_print_RR_query( N);
			fflush(stdout);
		}
#endif
			
		int new_come = 0;

		//if someone running
		if( running != -1 && proc[running].t_exec == 0 ){
			waitpid(proc[running].pid, NULL, 0); //already changed, return immediately

			//done
			printf("%s %d\n", proc[running].name, proc[running].pid);
			proc[running].pid = -1;
			RR_query[running] = -1;

			last_run = running;
			running = -1;
			finish_cnt++;

			if( finish_cnt == N) continue;
		}
		
		//if someone ready, given pid
		//same time: all ready
		for( int i = 0 ; i < N ; i++){
			if( proc[i].t_exec == 0 || proc[i].pid != -1 ) continue;
			if( proc[i].t_ready == ntime ){
				new_come = 1;
				proc[i].pid = proc_exec(proc[i]);
				proc_setscheduler(proc[i].pid, SCHED_IDLE);

				RR_query[i] = ntime;
			}
		}



		if( policy != RR && new_come == 1 ){
			//select one from algorithm, wake up next
			int next = next_process( proc, N, policy);
			//next == -1 : no job ready / all done
			if( next != -1 ){ 
				if( running != next ){
					proc_setscheduler(proc[next].pid, SCHED_OTHER); //wakeup
					proc_setscheduler(proc[running].pid, SCHED_IDLE); //block

					if( running != -1 ) last_run = running;
					running = next;
					rr_time_cumulate = 0;
				}
			}
		}
		else if( running == -1 ||  rr_time_cumulate%RR_TIMEQUANTUM == 0 ){
			//select one from algorithm, wake up next
			int next = next_process( proc, N, policy);
			//next == -1 : no job ready / all done
			if( next != -1 ){ 
				if( running != next ){
					proc_setscheduler(proc[next].pid, SCHED_OTHER); //wakeup
					proc_setscheduler(proc[running].pid, SCHED_IDLE); //block

					if( running != -1 ) last_run = running;
					running = next;
					rr_time_cumulate = 0;
				}
			}

		}

		UNIT_T(1); //scheduler also ellipse a time unit
		if( running != -1 )
			proc[running].t_exec--;
		ntime++; rr_time_cumulate++;
	}


	return 0;	
}
