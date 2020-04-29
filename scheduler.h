#ifndef _SCHEDULING_H_
#define _SCHEDULING_H_

#include "process.h"

#define FIFO	0
#define RR		1
#define SJF		2
#define PSJF	3
#define RR_TIMEQUANTUM 500

int scheduling(struct process *proc, int N, int policy);

#endif
