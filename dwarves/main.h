#ifndef MAINH
#define MAINH
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <math.h>
#include "../util.h"
#include "../queue.h"

/* tutaj TYLKO zapowiedzi - definicje w main.c */
extern int rank;
extern int size;
extern int ack_count;
extern int ack_portal_count;
extern int job_id;  // id zlecenia o ktore bedzie sie ubiegal dany krasnal
extern pthread_t com_thread;
extern long lamport_clock;
extern pthread_mutex_t clock_mut;
extern struct Queue* ack_queue; 

#endif
