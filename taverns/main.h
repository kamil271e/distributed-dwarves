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
#include <semaphore.h>
#include "../util.h"
#include "../queue.h"

/* tutaj TYLKO zapowiedzi - definicje w main.c */
extern int rank;
extern int size;
extern long lamport_clock;

extern pthread_t com_thread;
extern struct Queue* ack_queue; 

#endif
